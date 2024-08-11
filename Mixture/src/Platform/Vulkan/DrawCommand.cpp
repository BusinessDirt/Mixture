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
}
