#include "mxpch.hpp"
#include "RendererSystem.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    RendererSystem::RendererSystem()
    {
        m_Pipeline = GraphicsPipeline::Create("shader");
    }

    RendererSystem::~RendererSystem()
    {
        m_Pipeline = nullptr;
    }

    void RendererSystem::Update()
    {
        
    }

    void RendererSystem::Draw(CommandBuffer commandBuffer)
    {
        m_Pipeline->Bind(commandBuffer);
        
        // TODO: abstract into DrawCommand or smth
        // Draw(), DrawIndexed(), DrawIndirect(), DrawIndirectIndexed()
        vkCmdDraw(commandBuffer.GetAsVulkanHandle(), 3, 1, 0, 0);
    }
}
