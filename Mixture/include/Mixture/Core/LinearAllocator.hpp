#pragma once

/**
 * @file LinearAllocator.hpp
 * @brief A linear allocator implementation for efficient memory management.
 */

#include "Mixture/Core/Base.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace Mixture
{
    /**
     * @brief A simple linear allocator that allocates memory sequentially.
     *
     * Useful for per-frame allocations that can be reset all at once.
     */
    class LinearAllocator
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param size The total size of the allocator in bytes. Default is 256KB.
         */
        explicit LinearAllocator(size_t size = 256 * 1024);
        ~LinearAllocator();

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
            static_assert(std::is_trivially_destructible<PassData>::value, "RenderGraph PassData must be trivially destructible (POD). Do not use std::vector or std::string inside PassData!");

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
                OPAL_CRITICAL("Core", "LinearAllocator overflow! Increase size.");
                exit(-1);
            }

            // Update bump pointer
            m_Current = (void*)(nextAddress + size);

            // Construct the object in the reserved memory
            T* result = new ((void*)nextAddress) T(std::forward<Args>(args)...);
            return result;
        }

        /**
         * @brief Resets the allocator, invalidating all previous allocations.
         *
         * Should be called at the start of every frame or when the memory is no longer needed.
         */
        void Reset();

    private:
        void* m_Start = nullptr;
        void* m_Current = nullptr;
        size_t m_TotalSize = 0;
    };
}
