#include "mxpch.hpp"
#include "Mixture/Core/Memory/PoolAllocator.hpp"

#include <limits>
#include <new>

namespace Mixture
{
    PoolAllocator::PoolAllocator(size_t blockSize, size_t align, size_t blockCount)
        : m_BlockSize(blockSize), m_Alignment(align), m_BlockCount(blockCount), m_UsedCount(0)
    {
        if (blockSize == 0 || blockCount == 0)
            throw std::invalid_argument("PoolAllocator block size and count must be greater than zero");
        if (align == 0 || (align & (align - 1)) != 0)
            throw std::invalid_argument("PoolAllocator alignment must be a nonzero power of two");

        m_Alignment = std::max(m_Alignment, alignof(Node));
        // Ensure block size is at least the size of a pointer for the free list
        m_BlockSize = std::max(m_BlockSize, sizeof(Node));

        // Calculate actual stride per block including alignment
        if (m_BlockSize > std::numeric_limits<size_t>::max() - (m_Alignment - 1))
            throw std::overflow_error("PoolAllocator block stride overflow");
        m_BlockSize = (m_BlockSize + m_Alignment - 1) & ~(m_Alignment - 1);
        if (m_BlockCount > std::numeric_limits<size_t>::max() / m_BlockSize)
            throw std::overflow_error("PoolAllocator total size overflow");

        // Allocate the big chunk
        const size_t totalSize = m_BlockSize * m_BlockCount;
        m_MemoryBlock = ::operator new(totalSize, std::align_val_t(m_Alignment));
        m_Allocated.assign(m_BlockCount, false);

        Reset(); // Initialize free list
        OPAL_INFO("Core/Memory", "Initialized PoolAllocator: {} blocks of {} bytes (Total: {}KB)",
            m_BlockCount, m_BlockSize, totalSize / 1024.0f);
    }

    PoolAllocator::~PoolAllocator()
    {
        if (m_MemoryBlock)
        {
            ::operator delete(m_MemoryBlock, std::align_val_t(m_Alignment));
            m_MemoryBlock = nullptr;
        }
    }

    void* PoolAllocator::Allocate()
    {
        if (!m_FreeList)
        {
            OPAL_WARN("Core/Memory", "PoolAllocator exhausted! (Capacity: {})", m_BlockCount);
            return nullptr;
        }

        Node* node = m_FreeList;
        m_FreeList = node->Next;
        const size_t index = (reinterpret_cast<uintptr_t>(node) - reinterpret_cast<uintptr_t>(m_MemoryBlock)) / m_BlockSize;
        m_Allocated[index] = true;
        m_UsedCount++;

        return (void*)node;
    }

    bool PoolAllocator::IsAllocated(const void* ptr) const
    {
        if (!ptr || !m_MemoryBlock) return false;
        const uintptr_t start = reinterpret_cast<uintptr_t>(m_MemoryBlock);
        const uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
        const size_t totalSize = m_BlockSize * m_BlockCount;
        if (address < start || address - start >= totalSize) return false;
        const size_t offset = static_cast<size_t>(address - start);
        if (offset % m_BlockSize != 0) return false;
        return m_Allocated[offset / m_BlockSize];
    }

    bool PoolAllocator::Free(void* ptr)
    {
        if (!IsAllocated(ptr))
        {
            OPAL_ERROR("Core/Memory", "PoolAllocator::Free rejected a foreign, misaligned, or already-free pointer");
            return false;
        }

        const size_t index = (reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(m_MemoryBlock)) / m_BlockSize;
        m_Allocated[index] = false;

        Node* node = (Node*)ptr;
        node->Next = m_FreeList;
        m_FreeList = node;
        m_UsedCount--;
        return true;
    }

    void PoolAllocator::Reset()
    {
        m_UsedCount = 0;
        m_FreeList = nullptr;
        std::fill(m_Allocated.begin(), m_Allocated.end(), false);

        // Reconstruct the linked list inside the memory block
        uintptr_t current = (uintptr_t)m_MemoryBlock;
        for (size_t i = 0; i < m_BlockCount; i++)
        {
            Node* node = (Node*)current;
            
            if (i == m_BlockCount - 1)
                node->Next = nullptr;
            else
                node->Next = (Node*)(current + m_BlockSize);

            // If this is the first block, it's the head
            if (i == 0) m_FreeList = node;

            current += m_BlockSize;
        }
    }
}
