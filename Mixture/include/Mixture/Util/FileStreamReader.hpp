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
        FileStreamReader(const std::filesystem::path& path);

        bool IsOpen() const { return m_Stream.is_open(); }

        // Read bytes into a buffer
        void ReadBuffer(Vector<char>& buffer);

        // TODO: Add specific read methods (ReadInt, ReadString) as needed
        // ...

    private:
        std::ifstream m_Stream;
    };
}
