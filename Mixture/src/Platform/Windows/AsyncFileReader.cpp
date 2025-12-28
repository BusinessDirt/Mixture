#include "mxpch.hpp"
#include "Mixture/Util/AsyncFileReader.hpp"

#ifdef OPAL_PLATFORM_WINDOWS

#include <windows.h>

namespace Mixture
{
    struct AsyncFileReader::Impl
    {
        HANDLE FileHandle = INVALID_HANDLE_VALUE;
        size_t FileSize = 0;
        bool IsOpen = false;

        Impl(const std::filesystem::path& path)
        {
            // Open for Overlapped I/O
            FileHandle = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

            if (FileHandle != INVALID_HANDLE_VALUE)
            {
                IsOpen = true;
                LARGE_INTEGER size;
                if (GetFileSizeEx(FileHandle, &size))
                {
                    FileSize = static_cast<size_t>(size.QuadPart);
                }
            }
        }

        ~Impl()
        {
            if (FileHandle != INVALID_HANDLE_VALUE)
            {
                CloseHandle(FileHandle);
            }
        }
    };

    struct ReadContext
    {
        OVERLAPPED Overlapped;
        AsyncFileReader::ReadCallback Callback;
        Vector<char> Buffer;
    };

    static VOID CALLBACK FileIOCompletionRoutine(
      DWORD dwErrorCode,
      DWORD dwNumberOfBytesTransfered,
      LPOVERLAPPED lpOverlapped
    )
    {
        // Recover context. Overlapped is the first member, so reinterpret_cast is safe.
        auto* context = reinterpret_cast<ReadContext*>(lpOverlapped);

        if (dwErrorCode == 0)
        {
             if (dwNumberOfBytesTransfered < context->Buffer.size())
             {
                 context->Buffer.resize(dwNumberOfBytesTransfered);
             }
             context->Callback(std::move(context->Buffer));
        }
        else
        {
            context->Callback({});
        }

        delete context;
    }

    AsyncFileReader::AsyncFileReader(const std::filesystem::path& path)
        : m_Impl(new Impl(path))
    {
        if (m_Impl->IsOpen)
        {
            // Bind file handle to the system's I/O completion port thread pool
            BindIoCompletionCallback(m_Impl->FileHandle, FileIOCompletionRoutine, 0);
        }
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

        auto* context = new ReadContext();
        ZeroMemory(&context->Overlapped, sizeof(OVERLAPPED));
        context->Callback = std::move(callback);
        context->Buffer.resize(m_Impl->FileSize);

        // Initiate Async Read
        // Note: Offset is in Overlapped.Offset/OffsetHigh. We read from 0 by default (ZeroMemory).
        if (!ReadFile(m_Impl->FileHandle, context->Buffer.data(), static_cast<DWORD>(context->Buffer.size()), NULL, &context->Overlapped))
        {
            DWORD error = GetLastError();
            if (error != ERROR_IO_PENDING)
            {
                // Immediate failure
                // Invoke callback with empty buffer and cleanup
                // We use a temporary local callback to ensure we don't access context after delete
                ReadCallback cb = std::move(context->Callback);
                delete context;
                cb({});
            }
        }
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
