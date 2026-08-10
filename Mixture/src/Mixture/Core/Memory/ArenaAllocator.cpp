#include "mxpch.hpp"
#include "Mixture/Core/Memory/ArenaAllocator.hpp"

#include <new>

namespace Mixture
{
    ArenaAllocator::ArenaAllocator(size_t size)
        : m_TotalSize(size)
    {
        if (size == 0) throw std::invalid_argument("ArenaAllocator size must be greater than zero");
        m_Start = std::malloc(m_TotalSize);
        if (!m_Start) throw std::bad_alloc();
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
