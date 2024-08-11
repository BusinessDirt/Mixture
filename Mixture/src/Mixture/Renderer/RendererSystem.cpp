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
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
        };
        
        const std::vector<uint16_t> indices = 
        {
            0, 1, 2, 2, 3, 0
        };
        
        m_VertexBuffer = VertexBuffer::Create(vertices);
        m_IndexBuffer = IndexBuffer::Create(indices);
    }

    RendererSystem::~RendererSystem()
    {
        m_IndexBuffer = nullptr;
        m_VertexBuffer = nullptr;
        m_Pipeline = nullptr;
    }

    void RendererSystem::Update()
    {
        
    }

    void RendererSystem::Draw(CommandBuffer commandBuffer)
    {
        m_Pipeline->Bind(commandBuffer);
        
        m_VertexBuffer->Bind(commandBuffer);
        m_IndexBuffer->Bind(commandBuffer);
        
        DrawCommand::DrawIndexed(commandBuffer, m_IndexBuffer->GetIndexCount());
    }
}
