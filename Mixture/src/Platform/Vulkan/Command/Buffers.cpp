#include "mxpch.hpp"
#include "Platform/Vulkan/Command/Buffers.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    CommandBuffers::CommandBuffers(const VkCommandPool commandPool, const size_t size)
        : m_CommandPool(commandPool)
    {
        m_CommandBuffers.resize(size);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        VK_ASSERT(vkAllocateCommandBuffers(Context::Device->GetHandle(), &allocInfo, m_CommandBuffers.data()),
                  "Mixture::Vulkan::CommandBuffers::CommandBuffers() - Allocation failed!")
    }

    CommandBuffers::~CommandBuffers()
    {
        vkFreeCommandBuffers(Context::Device->GetHandle(), m_CommandPool, static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
        m_CommandBuffers.clear();
    }
}
