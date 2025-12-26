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
        if (!m_Stream.is_open()) return;
        m_Stream.seekg(0, std::ios::end);
        buffer.resize(m_Stream.tellg());
        m_Stream.seekg(0, std::ios::beg);
        m_Stream.read(buffer.data(), buffer.size());
    }

    bool FileStreamReader::ReadRaw(void* dest, size_t size)
    {
        if (!m_Stream.is_open()) return false;
        m_Stream.read(static_cast<char*>(dest), size);
        return m_Stream.good();
    }

    size_t FileStreamReader::GetFileSize()
    {
        if (!m_Stream.is_open()) return 0;
        std::streampos current = m_Stream.tellg();
        m_Stream.seekg(0, std::ios::end);
        size_t size = m_Stream.tellg();
        m_Stream.seekg(current); // Restore position
        return size;
    }
}
