#include "MainLayer.hpp"

#include "Mixture/Core/Application.hpp"

namespace Mixture
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Color;
    };

    void MainLayer::OnAttach()
    {
        OPAL_INFO("Client", "MainLayer::OnAttach()");

        // Create Vertex Buffer
        Vertex vertices[] = {
            { { 0.0f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            { {-0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
        };

        RHI::BufferDesc desc;
        desc.Size = sizeof(vertices);
        desc.Usage = RHI::BufferUsage::Vertex;
        desc.DebugName = "TriangleVB";

        m_VertexBuffer = Application::Get().GetContext().GetDevice().CreateBuffer(desc, vertices);
    }

    void MainLayer::OnDetach()
    {
        OPAL_INFO("Client", "MainLayer::OnDetach()");
        m_VertexBuffer.reset();
    }

    void MainLayer::OnEvent(Event& event)
    {

    }

    void MainLayer::OnUpdate(float dt)
    {

    }

    void MainLayer::OnRender(RenderGraph& graph)
    {
        struct GamePassData
        {
            RGResourceHandle Output;
            RHI::IPipeline* Pipeline;
        };

        graph.AddPass<GamePassData>("GBufferPass",
            [&](RenderGraphBuilder& builder, GamePassData& data)
            {
                // Import the vertex buffer so we can use it in the pass
                // Note: BindVertexBuffer doesn't strictly require the graph to know about it for simple cases,
                // but explicitly importing it is good practice for barrier handling.
                // However, the current RenderGraph implementation might not automatically transition it
                // unless we declare a Read/Write. For VertexBuffer usage, it's usually ReadOnly.
                // Let's import it to be safe.
                graph.ImportResource("TriangleVB", m_VertexBuffer.get());

                // Declare Writes
                RGResourceHandle backbuffer = graph.GetResource("Backbuffer");
                RGAttachmentInfo info;
                info.Handle = backbuffer;
                info.LoadOp = RHI::LoadOp::Clear;
                info.ClearColor[0] = 0.0f;
                info.ClearColor[1] = 0.0f;
                info.ClearColor[2] = 0.0f;
                info.ClearColor[3] = 1.0f;

                data.Output = builder.Write(info);

                // Define Pipeline Requirements (Shaders + State)
                RHI::PipelineDesc desc;
                desc.VertexShader = builder.LoadShader("Triangle.hlsl", RHI::ShaderStage::Vertex);
                desc.FragmentShader = builder.LoadShader("Triangle.hlsl", RHI::ShaderStage::Fragment);
                desc.Rasterizer.cullMode = RHI::CullMode::Front;
                data.Pipeline = builder.CreatePipeline(desc);
            },
            [&](const RenderGraphRegistry& registry, const GamePassData& data, RHI::ICommandList* cmd)
            {
                // Bind Pipeline and Draw
                cmd->BindPipeline(data.Pipeline);

                // Bind Vertex Buffer (Binding 0)
                if (m_VertexBuffer)
                    cmd->BindVertexBuffer(m_VertexBuffer.get(), 0);

                cmd->Draw(3, 1, 0, 0);
            }
        );

        graph.AddPass<GamePassData>("DebugPass",
            [&](RenderGraphBuilder& builder, GamePassData& data)
            {
                RGResourceHandle backbuffer = graph.GetResource("Backbuffer");
                RGAttachmentInfo info;
                info.Handle = backbuffer;
                info.LoadOp = RHI::LoadOp::Load;
                info.StoreOp = RHI::StoreOp::Store;
                data.Output = builder.Write(info);

                RHI::PipelineDesc desc;
                desc.VertexShader = builder.LoadShader("Triangle.hlsl", RHI::ShaderStage::Vertex);
                desc.FragmentShader = builder.LoadShader("Triangle.hlsl", RHI::ShaderStage::Fragment);
                desc.Rasterizer.cullMode = RHI::CullMode::Front;
                data.Pipeline = builder.CreatePipeline(desc);
            },
            [&](const RenderGraphRegistry& registry, const GamePassData& data, RHI::ICommandList* cmd)
            {
                float width = (float)Application::Get().GetContext().GetSwapchainWidth();
                float height = (float)Application::Get().GetContext().GetSwapchainHeight();

                cmd->SetViewport(width / 2.0f, height / 2.0f, width / 2.0f, height / 2.0f);
                cmd->SetScissor(0, 0, (uint32_t)width, (uint32_t)height);
                cmd->BindPipeline(data.Pipeline);

                if (m_VertexBuffer)
                    cmd->BindVertexBuffer(m_VertexBuffer.get(), 0);

                cmd->Draw(3, 1, 0, 0);
            }
        );
    }
}
