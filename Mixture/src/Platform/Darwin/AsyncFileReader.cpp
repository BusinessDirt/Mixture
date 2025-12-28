#include "mxpch.hpp"
#include "Mixture/Util/AsyncFileReader.hpp"

#ifdef OPAL_PLATFORM_DARWIN

#include <dispatch/dispatch.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

namespace Mixture
{
    struct AsyncFileReader::Impl
    {
        dispatch_io_t Channel = nullptr;
        dispatch_queue_t Queue = nullptr;
        size_t FileSize = 0;
        bool IsOpen = false;
        std::filesystem::path FilePath;

        Impl(const std::filesystem::path& path)
            : FilePath(path)
        {
            Queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

            struct stat st;
            if (stat(path.c_str(), &st) == 0)
            {
                FileSize = st.st_size;
                IsOpen = true;
            }
            else
            {
                IsOpen = false;
                return;
            }

            // Create channel
            // Note: dispatch_io_create_with_path opens the file for us.
            Channel = dispatch_io_create_with_path(DISPATCH_IO_STREAM, path.c_str(), O_RDONLY, 0, Queue, ^(int error) {
                // Cleanup handler (can be empty if we don't need to do anything specific)
            });

            if (!Channel) IsOpen = false;
        }

        ~Impl()
        {
            if (Channel)
            {
                dispatch_io_close(Channel, DISPATCH_IO_STOP);
                dispatch_release(Channel);
            }
            // Queue is global, do not release
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
        if (!m_Impl || !m_Impl->IsOpen || !m_Impl->Channel)
        {
            callback({});
            return;
        }

        size_t size = m_Impl->FileSize;

        // dispatch_io_read reads until 'length' is met or EOF
        dispatch_io_read(m_Impl->Channel, 0, size, m_Impl->Queue, ^(bool done, dispatch_data_t data, int error) {
            if (error)
            {
                callback({});
                return;
            }

            if (done)
            {
                if (!data) {
                    callback({});
                    return;
                }

                size_t dataSize = dispatch_data_get_size(data);
                Vector<char> buffer(dataSize);
                char* destPtr = buffer.data();

                // Copy data
                dispatch_data_apply(data, ^bool(dispatch_data_t region, size_t offset, const void *bufferPtr, size_t size) {
                    std::memcpy(destPtr + offset, bufferPtr, size);
                    return true;
                });

                callback(std::move(buffer));
            }
        });
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
