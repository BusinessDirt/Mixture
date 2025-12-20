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
        };

        graph.AddPass<GamePassData>("GBufferPass",
            [&](RenderGraphBuilder& builder, GamePassData& data)
            {
                RGResourceHandle backbuffer = graph.GetResource("Backbuffer");

                RGAttachmentInfo info;
                info.Handle = backbuffer;
                info.LoadOp = RHI::LoadOp::Clear;
                info.ClearColor[0] = 0.1f;
                info.ClearColor[1] = 0.1f;
                info.ClearColor[2] = 0.1f;
                info.ClearColor[3] = 1.0f;

                data.Output = builder.Write(info);
            },
            [&](RenderGraphRegistry& registry, GamePassData& data, Ref<RHI::ICommandList> cmd)
            {

            }
        );


    }
}
