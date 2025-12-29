#pragma once

/**
 * @file ArenaAllocator.hpp
 * @brief A linear (arena) allocator implementation for efficient temporary memory management.
 */

#include "Mixture/Core/Base.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace Mixture
{
    /**
     * @brief A simple arena allocator that allocates memory sequentially.
     *
     * Useful for per-frame allocations that can be reset all at once.
     * Allocation is O(1) (pointer bump). Deallocation is O(1) (reset pointer).
     * Individual frees are not supported.
     */
    class ArenaAllocator
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param size The total size of the arena in bytes. Default is 256KB.
         */
        explicit ArenaAllocator(size_t size = 256 * 1024);
        ~ArenaAllocator();

        // Non-copyable
        ArenaAllocator(const ArenaAllocator&) = delete;
        ArenaAllocator& operator=(const ArenaAllocator&) = delete;

        /**
         * @brief Allocates memory for an object of type T and constructs it.
         *
         * @tparam T The type of object to allocate.
         * @tparam Args Argument types for the constructor.
         * @param args Arguments for the constructor.
         * @return T* Pointer to the allocated object.
         */
        template<typename T, typename... Args>
        T* Alloc(Args&&... args)
        {
            // Calculate required alignment
            size_t size = sizeof(T);
            size_t alignment = alignof(T);

            // Calculate current address as integer
            uintptr_t currentAddress = (uintptr_t)m_Current;

            // Align the address (move forward to next multiple of alignment)
            size_t padding = (alignment - (currentAddress % alignment)) % alignment;
            uintptr_t nextAddress = currentAddress + padding;

            // Check overflow
            if (nextAddress + size > (uintptr_t)m_Start + m_TotalSize)
            {
                OPAL_CRITICAL("Core", "ArenaAllocator overflow! Allocated: {}KB. Total: {}KB", 
                    (GetUsedMemory() / 1024.0f), (m_TotalSize / 1024.0f));
                // In a robust engine, we might want to resize or return nullptr, but for now we crash.
                return nullptr;
            }

            // Update bump pointer
            m_Current = (void*)(nextAddress + size);

            // Construct the object in the reserved memory
            T* result = new ((void*)nextAddress) T(std::forward<Args>(args)...);
            return result;
        }

        /**
         * @brief Allocates raw bytes with specified alignment.
         */
        void* AllocRaw(size_t size, size_t alignment = 8)
        {
            uintptr_t currentAddress = (uintptr_t)m_Current;
            size_t padding = (alignment - (currentAddress % alignment)) % alignment;
            uintptr_t nextAddress = currentAddress + padding;

            if (nextAddress + size > (uintptr_t)m_Start + m_TotalSize)
            {
                OPAL_CRITICAL("Core", "ArenaAllocator overflow in AllocRaw!");
                return nullptr;
            }

            m_Current = (void*)(nextAddress + size);
            return (void*)nextAddress;
        }

        /**
         * @brief Resets the allocator, invalidating all previous allocations.
         *
         * Should be called at the start of every frame or when the memory is no longer needed.
         * Note: Destructors of allocated objects are NOT called.
         */
        void Reset();

        /**
         * @brief Returns the amount of memory currently used in bytes.
         */
        size_t GetUsedMemory() const { return (uintptr_t)m_Current - (uintptr_t)m_Start; }

        /**
         * @brief Returns the total size of the arena in bytes.
         */
        size_t GetTotalSize() const { return m_TotalSize; }

    private:
        void* m_Start = nullptr;
        void* m_Current = nullptr;
        size_t m_TotalSize = 0;
    };
}
