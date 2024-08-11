#include "mxpch.hpp"
#include "DrawCommand.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    void DrawCommand::DrawCmd(CommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount,
        uint32_t firstVertex, uint32_t firstInstance)
    {
        vkCmdDraw(commandBuffer.GetAsVulkanHandle(), vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void DrawCommand::DrawIndexedCmd(CommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount,
        uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
    {
        vkCmdDrawIndexed(commandBuffer.GetAsVulkanHandle(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }
}
