#include "mxpch.hpp"
#include "Mixture/Util/AsyncFileReader.hpp"

#ifdef OPAL_PLATFORM_LINUX

#include <linux/io_uring.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>
#include <cstring>
#include <atomic>

// Minimal syscall wrappers
static int io_uring_setup(unsigned entries, struct io_uring_params *p) {
    return (int)syscall(__NR_io_uring_setup, entries, p);
}

static int io_uring_enter(int fd, unsigned to_submit, unsigned min_complete, unsigned flags, sigset_t *sig) {
    return (int)syscall(__NR_io_uring_enter, fd, to_submit, min_complete, flags, sig);
}

namespace Mixture
{
    struct AsyncFileReader::Impl
    {
        int fd = -1;
        size_t FileSize = 0;
        bool IsOpen = false;

        Impl(const std::filesystem::path& path)
        {
            fd = open(path.c_str(), O_RDONLY);
            if (fd >= 0)
            {
                struct stat st;
                if (fstat(fd, &st) == 0)
                {
                    FileSize = st.st_size;
                    IsOpen = true;
                }
                else
                {
                    close(fd);
                    fd = -1;
                }
            }
        }

        ~Impl()
        {
            if (fd >= 0) close(fd);
        }
    };

    AsyncFileReader::AsyncFileReader(const std::filesystem::path& path)
        : m_Impl(new Impl(path))
    {
    }

    AsyncFileReader::~AsyncFileReader()
    {
        delete m_Impl;
    }

    AsyncFileReader::AsyncFileReader(AsyncFileReader&& other) noexcept
        : m_Impl(other.m_Impl)
    {
        other.m_Impl = nullptr;
    }

    AsyncFileReader& AsyncFileReader::operator=(AsyncFileReader&& other) noexcept
    {
        if (this != &other)
        {
            delete m_Impl;
            m_Impl = other.m_Impl;
            other.m_Impl = nullptr;
        }
        return *this;
    }

    void AsyncFileReader::ReadBuffer(ReadCallback callback)
    {
        if (!m_Impl || !m_Impl->IsOpen)
        {
            callback({});
            return;
        }

        int fileFd = m_Impl->fd;
        size_t fileSize = m_Impl->FileSize;

        // Spawn a thread to handle the io_uring operations
        std::thread([fileFd, fileSize, cb = std::move(callback)]() mutable {

            struct io_uring_params params;
            memset(&params, 0, sizeof(params));
            int ring_fd = io_uring_setup(1, &params);

            if (ring_fd < 0) {
                cb({});
                return;
            }

            // Map Ring Buffers
            int sring_sz = params.sq_off.array + params.sq_entries * sizeof(unsigned);
            int cring_sz = params.cq_off.cqes + params.cq_entries * sizeof(struct io_uring_cqe);

            if (params.features & IORING_FEAT_SINGLE_MMAP) {
                if (cring_sz > sring_sz) sring_sz = cring_sz;
                cring_sz = sring_sz;
            }

            void *sq_ptr = mmap(0, sring_sz, PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_POPULATE, ring_fd, IORING_OFF_SQ_RING);

            void *cq_ptr = nullptr;
            if (params.features & IORING_FEAT_SINGLE_MMAP) {
                cq_ptr = sq_ptr;
            } else {
                cq_ptr = mmap(0, cring_sz, PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_POPULATE, ring_fd, IORING_OFF_CQ_RING);
            }

            void *sqes = mmap(0, params.sq_entries * sizeof(struct io_uring_sqe),
                              PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ring_fd, IORING_OFF_SQES);

            if (sq_ptr == MAP_FAILED || cq_ptr == MAP_FAILED || sqes == MAP_FAILED) {
                // Cleanup maps and fd...
                close(ring_fd);
                cb({});
                return;
            }

            // Pointers to SQ variables
            unsigned *sq_tail = (unsigned*)((char*)sq_ptr + params.sq_off.tail);
            unsigned *sq_mask = (unsigned*)((char*)sq_ptr + params.sq_off.ring_mask);
            unsigned *sq_array = (unsigned*)((char*)sq_ptr + params.sq_off.array);

            // Allocate Buffer
            Vector<char> buffer(fileSize);

            // Prepare SQE
            struct io_uring_sqe *sqe = &((struct io_uring_sqe*)sqes)[*sq_tail & *sq_mask];
            memset(sqe, 0, sizeof(*sqe));
            sqe->opcode = IORING_OP_READ;
            sqe->fd = fileFd;
            sqe->addr = (unsigned long)buffer.data();
            sqe->len = (unsigned)fileSize;
            sqe->off = 0;

            // Submit
            *sq_tail = (*sq_tail) + 1; // Increment tail

            // Update array
            // Since we only have 1 entry and index 0, sq_array[0] = 0 (offset into sqes)
            unsigned index = (*sq_tail - 1) & *sq_mask;
            sq_array[index] = index; // Simple 1:1 mapping

            // Submit and Wait
            int ret = io_uring_enter(ring_fd, 1, 1, IORING_ENTER_GETEVENTS, NULL);

            if (ret < 0) {
                 // Error
                 cb({});
            } else {
                // Check CQ
                unsigned *cq_head = (unsigned*)((char*)cq_ptr + params.cq_off.head);
                unsigned *cq_tail = (unsigned*)((char*)cq_ptr + params.cq_off.tail);
                unsigned *cq_mask = (unsigned*)((char*)cq_ptr + params.cq_off.ring_mask);
                struct io_uring_cqe *cqes_ptr = (struct io_uring_cqe*)((char*)cq_ptr + params.cq_off.cqes);

                if (*cq_head != *cq_tail) {
                    struct io_uring_cqe *cqe = &cqes_ptr[*cq_head & *cq_mask];
                    if (cqe->res >= 0) {
                        // Success
                        // Resize buffer if partial read (unlikely for file unless EOF)
                        if ((size_t)cqe->res < buffer.size()) buffer.resize(cqe->res);
                        cb(std::move(buffer));
                    } else {
                        cb({});
                    }
                    *cq_head = (*cq_head) + 1; // Advance head

                     // Notify kernel we consumed CQE? Not strictly needed unless IORING_SETUP_CQSIZE was special or using advanced features, but usually good practice to update head.
                     io_uring_enter(ring_fd, 0, 0, IORING_ENTER_SQ_WAKEUP, NULL);
                } else {
                    cb({});
                }
            }

            // Cleanup
            munmap(sqes, params.sq_entries * sizeof(struct io_uring_sqe));
            if (cq_ptr && cq_ptr != sq_ptr) munmap(cq_ptr, cring_sz);
            if (sq_ptr) munmap(sq_ptr, sring_sz);
            close(ring_fd);

        }).detach();
    }

    bool AsyncFileReader::IsOpen() const
    {
        return m_Impl && m_Impl->IsOpen;
    }

    size_t AsyncFileReader::GetFileSize() const
    {
        return m_Impl ? m_Impl->FileSize : 0;
    }
}

#endif
