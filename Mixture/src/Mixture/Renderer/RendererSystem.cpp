#include "mxpch.hpp"
#include "RendererSystem.hpp"

#include "Mixture/Renderer/DrawCommand.hpp"

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
        DrawCommand::Draw(commandBuffer, 3);
    }
}
