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

        m_VertexBuffer = Application::Get().GetContext().GetDevice().CreateBuffer(desc, std::as_bytes(std::span(vertices)));
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
                desc.VertexShader = builder.LoadShader("Triangle.slang", RHI::ShaderStage::Vertex);
                desc.FragmentShader = builder.LoadShader("Triangle.slang", RHI::ShaderStage::Fragment);
                desc.Rasterizer.cullMode = RHI::CullMode::Front;
                data.Pipeline = builder.CreatePipeline(desc);
            },
            [&](const RenderGraphRegistry& registry, const GamePassData& data, RHI::ICommandList* cmd)
            {
                cmd->BindPipeline(data.Pipeline);
                if (m_VertexBuffer) cmd->BindVertexBuffer(m_VertexBuffer.get(), 0);
                cmd->Draw(3, 1, 0, 0);
            }
        );
    }
}
