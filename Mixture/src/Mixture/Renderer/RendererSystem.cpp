#include "mxpch.hpp"
#include "RendererSystem.hpp"

#include "Mixture/Renderer/DrawCommand.hpp"
#include "Mixture/Renderer/Buffer/VertexBuffer.hpp"

namespace Mixture
{
    

    RendererSystem::RendererSystem()
    {
        m_Pipeline = GraphicsPipeline::Create("shader");
        
        const std::vector<Vertex> vertices = 
        {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };
        
        m_VertexBuffer = VertexBuffer::Create(vertices);
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
        m_VertexBuffer->Bind(commandBuffer);
        DrawCommand::Draw(commandBuffer, m_VertexBuffer->GetVertexCount());
    }
}
