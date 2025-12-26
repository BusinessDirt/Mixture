#pragma once

/**
 * @file PoolAllocator.hpp
 * @brief A pool allocator implementation for efficient management of fixed-size objects.
 */

#include "Mixture/Core/Base.hpp"

#include <vector>
#include <cassert>

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
        void Free(void* ptr);

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
            // Verify size matches roughly (can't be exact due to alignment padding potentially)
            // But strict check:
            assert(sizeof(T) <= m_BlockSize && "Object too large for this pool!");

            void* mem = Allocate();
            if (!mem) return nullptr;
            return new (mem) T(std::forward<Args>(args)...);
        }

        /**
         * @brief Helper to destroy and free an object.
         */
        template<typename T>
        void Destroy(T* ptr)
        {
            if (ptr)
            {
                ptr->~T();
                Free(ptr);
            }
        }

    private:
        struct Node
        {
            Node* Next;
        };

        size_t m_BlockSize;
        size_t m_Alignment;
        size_t m_BlockCount;
        size_t m_UsedCount;

        void* m_MemoryBlock = nullptr;
        Node* m_FreeList = nullptr;
    };
}
