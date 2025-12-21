#include "mxpch.hpp"
#include "Mixture/Core/LinearAllocator.hpp"

namespace Mixture
{
        LinearAllocator::LinearAllocator(size_t size)
            : m_TotalSize(size)
        {
            m_Start = std::malloc(m_TotalSize);
            m_Current = m_Start;
            OPAL_INFO("Core", "Initalized Linear Allocator with {}KB of space", (m_TotalSize / 1024.0f));
        }

        LinearAllocator::~LinearAllocator()
        {
            std::free(m_Start);
        }

        void LinearAllocator::Reset()
        {
            m_Current = m_Start;
        }
}
