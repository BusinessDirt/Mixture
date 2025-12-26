#include "mxpch.hpp"
#include "Mixture/Core/Memory/ArenaAllocator.hpp"

namespace Mixture
{
    ArenaAllocator::ArenaAllocator(size_t size)
        : m_TotalSize(size)
    {
        m_Start = std::malloc(m_TotalSize);
        m_Current = m_Start;
        OPAL_INFO("Core/Memory", "Initialized Arena Allocator with {}KB of space", (m_TotalSize / 1024.0f));
    }

    ArenaAllocator::~ArenaAllocator()
    {
        if (m_Start)
        {
            std::free(m_Start);
            m_Start = nullptr;
        }
    }

    void ArenaAllocator::Reset()
    {
        m_Current = m_Start;
    }
}
