#pragma once
#include "Mixture/Core/Base.hpp"

#include "Platform/Vulkan/Definitions.hpp"

namespace Mixture::Vulkan
{
    class CommandPool
    {
    public:
        CommandPool(vk::Device device, const QueueFamilyIndices& indices);
        ~CommandPool();

        vk::CommandPool GetHandle() const { return m_Handle; }
    private:
        vk::Device m_Device;
        vk::CommandPool m_Handle;
    };
}
