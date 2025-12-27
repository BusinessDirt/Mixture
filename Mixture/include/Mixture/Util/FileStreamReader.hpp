#pragma once
#include "Mixture/Core/Base.hpp"

#include <fstream>
#include <vector>

namespace Mixture
{

    // wrapper around std::ifstream or a memory buffer
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
