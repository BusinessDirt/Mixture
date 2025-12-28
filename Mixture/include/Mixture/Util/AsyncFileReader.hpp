#pragma once

/**
 * @file AsyncFileReader.hpp
 * @brief Utility class for reading files asynchronously.
 */

#include "Mixture/Core/Base.hpp"

#include <filesystem>
#include <functional>
#include <vector>

namespace Mixture
{
    /**
     * @brief Class for reading files asynchronously using platform-specific APIs.
     *        (IOCP on Windows, io_dispatch on MacOS, io_uring on Linux)
     */
    class AsyncFileReader
    {
    public:
        // Callback signature: receives the data buffer. 
        // If vector is empty, read might have failed or file is empty.
        using ReadCallback = std::function<void(Vector<char>)>;

        /**
         * @brief Constructs an AsyncFileReader for the specified file.
         * 
         * @param path The path to the file.
         */
        AsyncFileReader(const std::filesystem::path& path);
        ~AsyncFileReader();

        AsyncFileReader(const AsyncFileReader&) = delete;
        AsyncFileReader& operator=(const AsyncFileReader&) = delete;

        AsyncFileReader(AsyncFileReader&&) noexcept;
        AsyncFileReader& operator=(AsyncFileReader&&) noexcept;

        /**
         * @brief Initiates an asynchronous read of the entire file.
         * @param callback The function to call when reading is complete.
         */
        void ReadBuffer(ReadCallback callback);

        /**
         * @brief Checks if the file was successfully opened.
         */
        bool IsOpen() const;

        /**
         * @brief Returns the size of the file.
         */
        size_t GetFileSize() const;

    private:
        struct Impl;
        Impl* m_Impl = nullptr;
    };
}
