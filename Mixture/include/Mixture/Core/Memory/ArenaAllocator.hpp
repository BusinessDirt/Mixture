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
            void* previous = m_Current;
            void* memory = AllocRaw(sizeof(T), alignof(T));
            if (!memory) return nullptr;
            try
            {
                return new (memory) T(std::forward<Args>(args)...);
            }
            catch (...)
            {
                m_Current = previous;
                throw;
            }
        }

        /**
         * @brief Allocates raw bytes with specified alignment.
         */
        void* AllocRaw(size_t size, size_t alignment = 8)
        {
            if (alignment == 0 || (alignment & (alignment - 1)) != 0)
                throw std::invalid_argument("ArenaAllocator alignment must be a nonzero power of two");
            if (!m_Start) return nullptr;

            const uintptr_t currentAddress = reinterpret_cast<uintptr_t>(m_Current);
            const size_t used = GetUsedMemory();
            const size_t remaining = m_TotalSize - used;
            const size_t padding = (alignment - (currentAddress & (alignment - 1))) & (alignment - 1);
            if (padding > remaining || size > remaining - padding) return nullptr;

            auto* result = static_cast<std::byte*>(m_Current) + padding;
            m_Current = result + size;
            return result;
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
