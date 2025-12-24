#include "mxpch.hpp"
#include "Mixture/Util/FileStreamReader.hpp"

namespace Mixture
{
    FileStreamReader::FileStreamReader(const std::filesystem::path& path)
    {
        m_Stream.open(path, std::ios::binary);
    }

    void FileStreamReader::ReadBuffer(Vector<char>& buffer)
    {
        m_Stream.seekg(0, std::ios::end);
        buffer.resize(m_Stream.tellg());
        m_Stream.seekg(0, std::ios::beg);
        m_Stream.read(buffer.data(), buffer.size());
    }
}
