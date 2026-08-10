#pragma once

/**
 * @file PoolAllocator.hpp
 * @brief A pool allocator implementation for efficient management of fixed-size objects.
 */

#include "Mixture/Core/Base.hpp"

#include <vector>
#include <stdexcept>

namespace Mixture
{
    /**
     * @brief A fixed-size block allocator.
     * 
     * Efficiently allocates and deallocates memory blocks of a specific size.
     * Ideal for particle systems, node allocations, or any frequent creation/destruction of uniform objects.
     * 
     * Uses a free-list to track available blocks. Allocation/Deallocation is O(1).
     */
    class PoolAllocator
    {
    public:
        /**
         * @brief Constructor.
         * 
         * @param blockSize Size of each block in bytes.
         * @param align Alignment of each block.
         * @param blockCount Total number of blocks to pre-allocate.
         */
        PoolAllocator(size_t blockSize, size_t align, size_t blockCount);
        ~PoolAllocator();

        // Non-copyable
        PoolAllocator(const PoolAllocator&) = delete;
        PoolAllocator& operator=(const PoolAllocator&) = delete;

        /**
         * @brief Allocates a single block.
         * 
         * @return void* Pointer to the allocated block, or nullptr if full.
         */
        void* Allocate();

        /**
         * @brief Returns a block to the pool.
         * 
         * @param ptr Pointer to the block to free.
         */
        bool Free(void* ptr);

        /**
         * @brief Resets the pool, marking all blocks as free.
         * Does NOT call destructors on objects!
         */
        void Reset();

        size_t GetBlockSize() const { return m_BlockSize; }
        size_t GetCapacity() const { return m_BlockCount; }
        size_t GetUsedCount() const { return m_UsedCount; }

        /**
         * @brief Helper to allocate and construct an object.
         * Assumes the pool was initialized with sizeof(T).
         */
        template<typename T, typename... Args>
        T* Create(Args&&... args)
        {
            if (sizeof(T) > m_BlockSize || alignof(T) > m_Alignment)
                throw std::invalid_argument("Object size or alignment exceeds the pool block contract");

            void* mem = Allocate();
            if (!mem) return nullptr;
            try
            {
                return new (mem) T(std::forward<Args>(args)...);
            }
            catch (...)
            {
                Free(mem);
                throw;
            }
        }

        /**
         * @brief Helper to destroy and free an object.
         */
        template<typename T>
        void Destroy(T* ptr)
        {
            if (ptr)
            {
                if (!IsAllocated(ptr))
                {
                    Free(ptr);
                    return;
                }
                ptr->~T();
                Free(ptr);
            }
        }

    private:
        struct Node
        {
            Node* Next;
        };

        bool IsAllocated(const void* ptr) const;

        size_t m_BlockSize;
        size_t m_Alignment;
        size_t m_BlockCount;
        size_t m_UsedCount;

        void* m_MemoryBlock = nullptr;
        Node* m_FreeList = nullptr;
        Vector<bool> m_Allocated;
    };
}
