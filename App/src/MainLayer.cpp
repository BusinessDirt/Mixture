#include "MainLayer.hpp"

namespace Mixture
{
    void MainLayer::OnAttach()
    {
        OPAL_INFO("Client", "MainLayer::OnAttach()");
    }

    void MainLayer::OnDetach()
    {
        OPAL_INFO("Client", "MainLayer::OnDetach()");
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
                // Declare Writes
                RGResourceHandle backbuffer = graph.GetResource("Backbuffer");
                RGAttachmentInfo info;
                info.Handle = backbuffer;
                info.LoadOp = RHI::LoadOp::Clear;
                info.ClearColor[0] = 0.1f;
                info.ClearColor[1] = 0.1f;
                info.ClearColor[2] = 0.1f;
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
                cmd->Draw(3, 1, 0, 0);
            }
        );
    }
}
