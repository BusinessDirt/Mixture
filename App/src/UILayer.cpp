#include "UILayer.hpp"

namespace Mixture
{
    void UILayer::OnAttach()
    {
        OPAL_INFO("Client", "UILayer::OnAttach()");
    }

    void UILayer::OnDetach()
    {
        OPAL_INFO("Client", "UILayer::OnDetach()");
    }

    void UILayer::OnEvent(Event& event)
    {

    }

    void UILayer::OnUpdate(float dt)
    {

    }

    void UILayer::OnRender(RenderGraph& graph)
    {
        struct UIPassData {
            RGResourceHandle SceneInput;
            RGResourceHandle Backbuffer;
        };

        graph.AddPass<UIPassData>("ImGuiPass",
            [&](RenderGraphBuilder& builder, UIPassData& data)
            {
                // Look for "SceneColor" created by GameLayer
                /*
                RGResourceHandle sceneHandle = graph.GetResource("SceneColor");
                if (sceneHandle.IsValid()) {
                    data.SceneInput = builder.Read(sceneHandle);
                }

                // Look for "Backbuffer" imported by Application
                RGResourceHandle backbufferHandle = graph.GetResource("Backbuffer");
                data.Backbuffer = builder.Write(backbufferHandle);
                */
            },
            [&](RenderGraphRegistry& registry, UIPassData& data, Ref<RHI::ICommandList> cmd)
            {
                // EXECUTE: Draw ImGui
                // Because we declared a Read on SceneInput, the Graph AUTOMATICALLY
                // inserts a barrier here to ensure SceneColor is safe to sample!

                // DrawImGui(registry.GetTexture(data.SceneInput));
            }
        );
    }
}
