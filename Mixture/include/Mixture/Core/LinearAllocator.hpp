#pragma once
#include "Mixture/Core/Base.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace Mixture
{
    class LinearAllocator
    {
    public:
        // default size: 256KB (plenty for pass data)
        explicit LinearAllocator(size_t size = 256 * 1024);
        ~LinearAllocator();

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
                OPAL_CRITICAL("Core", "LinearAllocator overflow! Increase size.");
                exit(-1);
            }

            // Update bump pointer
            m_Current = (void*)(nextAddress + size);

            // Construct the object in the reserved memory
            T* result = new ((void*)nextAddress) T(std::forward<Args>(args)...);
            return result;
        }

        // Called at the start of every frame
        void Reset();

    private:
        void* m_Start = nullptr;
        void* m_Current = nullptr;
        size_t m_TotalSize = 0;
    };
}
