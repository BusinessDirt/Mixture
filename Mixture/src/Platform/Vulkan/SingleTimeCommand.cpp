#include "mxpch.hpp"
#include "Platform/Vulkan/SingleTimeCommand.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Command/Pool.hpp"

namespace Mixture::Vulkan
{
    vk::CommandBuffer SingleTimeCommand::Begin()
    {
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = Context::Get().GetCommandPool().GetHandle();
        allocInfo.commandBufferCount = 1;

        vk::CommandBuffer commandBuffer = Context::Get().GetLogicalDevice().GetHandle().allocateCommandBuffers(allocInfo)[0];
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        commandBuffer.begin(beginInfo);

        return commandBuffer;
    }

    void SingleTimeCommand::End(const vk::CommandBuffer commandBuffer)
    {
        commandBuffer.end();

        vk::SubmitInfo submitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        auto device = Context::Get().GetLogicalDevice();

        device.GetGraphicsQueue().submit(submitInfo, nullptr);
        device.GetHandle().waitIdle();
        device.GetHandle().freeCommandBuffers(Context::Get().GetCommandPool().GetHandle(), commandBuffer);
    }

    void SingleTimeCommand::Submit(const std::function<void(vk::CommandBuffer)>& action)
    {
        const vk::CommandBuffer commandBuffer = Begin();
        action(commandBuffer);
        End(commandBuffer);
    }
}
