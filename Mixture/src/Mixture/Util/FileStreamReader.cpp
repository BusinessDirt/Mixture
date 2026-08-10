#include "mxpch.hpp"
#include "Mixture/Util/FileStreamReader.hpp"

#include <limits>

namespace Mixture
{
    FileStreamReader::FileStreamReader(const std::filesystem::path& path)
    {
        m_Stream.open(path, std::ios::binary);
    }

    bool FileStreamReader::ReadBuffer(Vector<char>& buffer)
    {
        if (!m_Stream.is_open()) return false;
        m_Stream.seekg(0, std::ios::end);
        const std::streampos end = m_Stream.tellg();
        if (end < 0 || static_cast<uintmax_t>(end) > std::numeric_limits<size_t>::max()
            || static_cast<uintmax_t>(end) > static_cast<uintmax_t>(std::numeric_limits<std::streamsize>::max()))
            return false;
        buffer.resize(static_cast<size_t>(end));
        m_Stream.seekg(0, std::ios::beg);
        m_Stream.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        if (m_Stream.gcount() != static_cast<std::streamsize>(buffer.size()))
        {
            buffer.clear();
            return false;
        }
        return true;
    }

    bool FileStreamReader::ReadRaw(void* dest, size_t size)
    {
        if (!m_Stream.is_open()) return false;
        if (size > static_cast<size_t>(std::numeric_limits<std::streamsize>::max())) return false;
        m_Stream.read(static_cast<char*>(dest), static_cast<std::streamsize>(size));
        return m_Stream.gcount() == static_cast<std::streamsize>(size);
    }

    size_t FileStreamReader::GetFileSize()
    {
        if (!m_Stream.is_open()) return 0;
        const std::streampos current = m_Stream.tellg();
        if (current < 0) return 0;
        m_Stream.seekg(0, std::ios::end);
        const std::streampos end = m_Stream.tellg();
        m_Stream.seekg(current); // Restore position
        if (end < 0 || static_cast<uintmax_t>(end) > std::numeric_limits<size_t>::max()) return 0;
        return static_cast<size_t>(end);
    }
}
