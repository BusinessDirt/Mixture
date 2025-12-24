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
            Ref<RHI::IPipeline> Pipeline;
        };

        graph.AddPass<GamePassData>("GBufferPass",
            [&](RenderGraphBuilder& builder, GamePassData& data)
            {
                // 1. Declare Writes (as you were doing)
                RGResourceHandle backbuffer = graph.GetResource("Backbuffer");
                RGAttachmentInfo info;
                info.Handle = backbuffer;
                info.LoadOp = RHI::LoadOp::Clear;
                // ... set clear color ...
                data.Output = builder.Write(info);

                // 2. Define Pipeline Requirements (Shaders + State)
                RHI::PipelineDesc desc;
                // Assume you have handles or pointers to shaders
                desc.VertexShader = myVertexShader.get();
                desc.FragmentShader = myPixelShader.get();
                desc.Rasterizer.cullMode = RHI::CullMode::Back;
                // You do NOT need to set ColorAttachmentFormats/DepthAttachmentFormat manually!

                // 3. Create (or Retrieve) the Pipeline
                // The builder will fill in the formats based on 'data.Output' above.
                data.Pipeline = builder.CreatePipeline(desc);
            },
            [&](const RenderGraphRegistry& registry, const GamePassData& data, Ref<RHI::ICommandList> cmd)
            {
                // 4. Bind Pipeline and Draw
                cmd->BindPipeline(data.Pipeline);
                cmd->Draw(3, 1, 0, 0);
            }
        );
    }
}
