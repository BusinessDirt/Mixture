#include "mxpch.hpp"
#include "Platform/Vulkan/Command/Pool.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    CommandPool::CommandPool()
    {
        const auto [Graphics, Present] = Context::PhysicalDevice->GetQueueFamilyIndices();

        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = Graphics.value();

        VK_ASSERT(vkCreateCommandPool(Context::Device->GetHandle(), &createInfo, nullptr, &m_CommandPool),
                  "Mixture::Vulkan::CommandPool::CommandPool() - Creation failed!")
    }

    CommandPool::~CommandPool()
    {
        vkDestroyCommandPool(Context::Device->GetHandle(), m_CommandPool, nullptr);
    }
}
