#include "mxpch.hpp"
#include "RendererSystem.hpp"

#include "Mixture/Renderer/DrawCommand.hpp"
#include "Mixture/Renderer/Buffer/VertexBuffer.hpp"
#include "Mixture/Core/Application.hpp"

#include <chrono>

#include "Platform/Vulkan/Context.hpp"

namespace Mixture
{

    struct UniformBufferObject
    {
        glm::mat4 View;
        glm::mat4 Proj;
    };

    struct PushConstant
    {
        glm::mat4 Model;
    };
    

    RendererSystem::RendererSystem()
    {
        m_Pipeline = GraphicsPipeline::Create("shader");
        
        const std::vector<Vertex> vertices = 
        {
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

            {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
        };

        const std::vector<uint16_t> indices = 
        {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4
        };
        
        m_VertexBuffer = VertexBuffer::Create(vertices);
        m_IndexBuffer = IndexBuffer::Create(indices);
        
        // TODO: figure out some sort of system that creates the descriptors automatically
        // and also creates the uniform buffers that can just be referenced here instead
        const ShaderInformation& shaderInfo = Application::Get().GetAssetManager().GetShader("shader.vert").GetInformation();
        
        m_UniformBuffer = UniformBuffer::Create(*shaderInfo.UniformBuffers[0]);
    }

    RendererSystem::~RendererSystem()
    {
        m_UniformBuffer = nullptr;
        m_IndexBuffer = nullptr;
        m_VertexBuffer = nullptr;
        m_Pipeline = nullptr;
    }

    void RendererSystem::Update(const FrameInfo& frameInfo)
    {
        UniformBufferObject ubo{};
        ubo.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.Proj = glm::perspective(glm::radians(45.0f), Application::Get().GetWindow().GetWidth() / (float) Application::Get().GetWindow().GetHeight(), 0.1f, 10.0f);
        ubo.Proj[1][1] *= -1;
        
        m_UniformBuffer->Update(&ubo);
    }

    void RendererSystem::Draw(const FrameInfo& frameInfo)
    {
        m_Pipeline->Bind(frameInfo);
        
        PushConstant push{};
        push.Model = glm::rotate(glm::mat4(1.0f), frameInfo.FrameTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        m_Pipeline->PushConstants(frameInfo, &push);
        
        m_VertexBuffer->Bind(frameInfo.CommandBuffer);
        m_IndexBuffer->Bind(frameInfo.CommandBuffer);
        
        DrawCommand::DrawIndexed(frameInfo.CommandBuffer, m_IndexBuffer->GetIndexCount());
    }
}
