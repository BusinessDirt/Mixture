#pragma once

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    class CommandPool
    {
    public:
        CommandPool(Device& device, const QueueFamilyIndices& indices);
        ~CommandPool();

        vk::CommandPool GetHandle() const { return m_Handle; }
    private:
        Device* m_Device;
        vk::CommandPool m_Handle;
    };
}
