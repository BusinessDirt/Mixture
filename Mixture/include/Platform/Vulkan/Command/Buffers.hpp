#pragma once
#include "Mixture/Core/Base.hpp"

#include "Platform/Vulkan/Definitions.hpp"

namespace Mixture::Vulkan
{
    class CommandBuffers
    {
    public:
        CommandBuffers(vk::Device device, vk::CommandPool commandPool, uint32_t count);
        ~CommandBuffers() = default;

        vk::CommandBuffer Get(uint32_t index) const { return m_Handles[index]; }
        vk::CommandBuffer* GetPointer(uint32_t index) { return &m_Handles[index]; }

        void Reset(uint32_t index);

    private:
        vk::Device m_Device;
        vk::CommandPool m_CommandPool;

        Vector<vk::CommandBuffer> m_Handles;
    };
}
