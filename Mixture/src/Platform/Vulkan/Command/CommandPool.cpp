#include "mxpch.hpp"
#include "CommandPool.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    CommandPool::CommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = Context::Get().PhysicalDevice->FindQueueFamilyIndices();

        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = queueFamilyIndices.Graphics.value();
        
        MX_VK_ASSERT(vkCreateCommandPool(Context::Get().Device->GetHandle(), &createInfo, nullptr, &m_CommandPool),
            "Failed to create VkCommandPool");
    }

    CommandPool::~CommandPool()
    {
        if (m_CommandPool)
        {
            vkDestroyCommandPool(Context::Get().Device->GetHandle(), m_CommandPool, nullptr);
            m_CommandPool = nullptr;
        }
    }
}
