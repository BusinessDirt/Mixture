#include "mxpch.hpp"
#include "Platform/Vulkan/SingleTimeCommand.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Command/Pool.hpp"

namespace Mixture::Vulkan
{
    vk::CommandBuffer SingleTimeCommand::Begin()
    {
        return Begin(Context::Get().GetGraphicsQueue());
    }

    vk::CommandBuffer SingleTimeCommand::Begin(Queue& queue)
    {
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = queue.GetPool().GetHandle();
        allocInfo.commandBufferCount = 1;

        vk::CommandBuffer commandBuffer = Context::Get().GetLogicalDevice().GetHandle().allocateCommandBuffers(allocInfo)[0];
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        commandBuffer.begin(beginInfo);

        return commandBuffer;
    }

    void SingleTimeCommand::End(const vk::CommandBuffer commandBuffer)
    {
        End(commandBuffer, Context::Get().GetGraphicsQueue());
    }

    void SingleTimeCommand::End(const vk::CommandBuffer commandBuffer, Queue& queue)
    {
        commandBuffer.end();

        auto device = Context::Get().GetLogicalDevice().GetHandle();

        vk::SubmitInfo submitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        // Create a fence to wait for just THIS command, not the whole GPU
        vk::FenceCreateInfo fenceInfo;
        vk::Fence fence = device.createFence(fenceInfo);

        queue.GetHandle().submit(submitInfo, fence);

        auto result = device.waitForFences(1, &fence, VK_TRUE, 100000000000); // 100s timeout
        if (result != vk::Result::eSuccess)
        {
            OPAL_ERROR("Core/Vulkan", "SingleTimeCommand::End() - Failed to wait for fence!");
        }

        device.destroyFence(fence);
        device.freeCommandBuffers(queue.GetPool().GetHandle(), commandBuffer);
    }

    void SingleTimeCommand::Submit(const std::function<void(vk::CommandBuffer)>& action)
    {
        Submit(Context::Get().GetGraphicsQueue(), action);
    }

    void SingleTimeCommand::Submit(Queue& queue, const std::function<void(vk::CommandBuffer)>& action)
    {
        const vk::CommandBuffer commandBuffer = Begin(queue);
        action(commandBuffer);
        End(commandBuffer, queue);
    }
}
