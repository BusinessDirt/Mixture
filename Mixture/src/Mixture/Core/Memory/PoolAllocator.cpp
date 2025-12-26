#include "mxpch.hpp"
#include "Mixture/Core/Memory/PoolAllocator.hpp"

namespace Mixture
{
    PoolAllocator::PoolAllocator(size_t blockSize, size_t align, size_t blockCount)
        : m_BlockSize(blockSize), m_Alignment(align), m_BlockCount(blockCount), m_UsedCount(0)
    {
        // Ensure block size is at least the size of a pointer for the free list
        if (m_BlockSize < sizeof(Node*))
            m_BlockSize = sizeof(Node*);

        // Calculate actual stride per block including alignment
        size_t alignmentPadding = (m_Alignment - (m_BlockSize % m_Alignment)) % m_Alignment;
        m_BlockSize += alignmentPadding;

        // Allocate the big chunk
        size_t totalSize = m_BlockSize * m_BlockCount;
        m_MemoryBlock = std::malloc(totalSize);
        
        if (!m_MemoryBlock)
        {
            OPAL_CRITICAL("Core/Memory", "PoolAllocator failed to allocate {} bytes", totalSize);
            return;
        }

        Reset(); // Initialize free list
        OPAL_INFO("Core/Memory", "Initialized PoolAllocator: {} blocks of {} bytes (Total: {}KB)",
            m_BlockCount, m_BlockSize, totalSize / 1024.0f);
    }

    PoolAllocator::~PoolAllocator()
    {
        if (m_MemoryBlock)
        {
            std::free(m_MemoryBlock);
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
        m_UsedCount++;

        return (void*)node;
    }

    void PoolAllocator::Free(void* ptr)
    {
        if (!ptr) return;

        // Verify ptr is within range (optional debug check)
        #ifdef OPAL_DEBUG
        uintptr_t start = (uintptr_t)m_MemoryBlock;
        uintptr_t end = start + (m_BlockSize * m_BlockCount);
        uintptr_t p = (uintptr_t)ptr;
        if (p < start || p >= end)
        {
            OPAL_ERROR("Core/Memory", "PoolAllocator::Free: Pointer out of bounds!");
            return;
        }
        #endif

        Node* node = (Node*)ptr;
        node->Next = m_FreeList;
        m_FreeList = node;
        m_UsedCount--;
    }

    void PoolAllocator::Reset()
    {
        m_UsedCount = 0;
        m_FreeList = nullptr;

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
