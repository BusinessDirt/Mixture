#pragma once

/**
 * @file FileStreamReader.hpp
 * @brief Utility class for reading files.
 */

#include "Mixture/Core/Base.hpp"

#include <fstream>
#include <vector>

namespace Mixture
{

    /**
     * @brief Wrapper around std::ifstream or a memory buffer for reading files.
     */
    class FileStreamReader
    {
    public:
        /**
         * @brief Constructs a FileStreamReader for the specified file.
         * 
         * @param path Path to the file to read.
         */
        FileStreamReader(const std::filesystem::path& path);

        /**
         * @brief Checks if the file stream is currently open.
         * 
         * @return true If open.
         */
        bool IsOpen() const { return m_Stream.is_open(); }

        /**
         * @brief Reads the entire file content into a vector.
         * 
         * @param buffer The destination buffer.
         */
        void ReadBuffer(Vector<char>& buffer);
        
        /**
         * @brief Reads a specific number of bytes into a raw pointer.
         * 
         * @param dest Destination memory address.
         * @param size Number of bytes to read.
         * @return true If read was successful.
         */
        bool ReadRaw(void* dest, size_t size);
        
        /**
         * @brief Gets the total size of the file in bytes.
         * 
         * @return size_t File size.
         */
        size_t GetFileSize();

        // TODO: Add specific read methods (ReadInt, ReadString) as needed
        // ...

    private:
        std::ifstream m_Stream;
    };
}
