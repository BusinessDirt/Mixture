#pragma once

#include "Mixture/Core/Base.hpp"

typedef struct VkCommandBuffer_T* VkCommandBuffer;

namespace Mixture
{
    /**
     * Wrapper class for the Handle of the specific api thats being used
     */
    class CommandBuffer
    {
    public:
        CommandBuffer();
        CommandBuffer(void* handle);
        ~CommandBuffer() = default;
        
        VkCommandBuffer GetAsVulkanHandle() const;
        //GetOpenGLHandle();
        //GetDirectX12Handle();
        
        explicit operator bool() const
        {
            return m_Handle;
        }
        
        bool operator==(const CommandBuffer& other) const
        {
            return m_Handle == other.m_Handle;
        }
        
    private:
        void* m_Handle;
    };
}
