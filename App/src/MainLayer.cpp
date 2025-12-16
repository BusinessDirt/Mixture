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
        struct GamePassData {
            RGResourceHandle Output;
        };

        graph.AddPass<GamePassData>("GBufferPass",
            [&](RenderGraphBuilder& builder, GamePassData& data) 
            {
                // Create a new texture named "SceneColor"
                TextureDesc desc;
                desc.Width = 1280; 
                desc.Height = 720; 
                desc.Format = Format::R8G8B8A8_UNORM;
                
                data.Output = builder.Create("SceneColor", desc);
                builder.Write(data.Output);
            },
            [&](RenderGraphRegistry& registry, GamePassData& data, ICommandList* cmd) 
            {
                // EXECUTE: Draw the scene
                // cmd->BindPipeline(...);
                // cmd->Draw(...);
            }
        );
    }
}
