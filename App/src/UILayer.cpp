#include "UILayer.hpp"

namespace Mixture
{
    void UILayer::OnAttach()
    {

    }

    void UILayer::OnDetach()
    {

    }

    void UILayer::OnEvent(Event& event)
    {

    }

    void UILayer::OnUpdate(float dt)
    {

    }

    void UILayer::OnRender(RenderGraph& graph)
    {
        graph.AddPass<ImGuiPass>("ImGuiPass", graph.GetResource("Backbuffer"));
    }

    void UILayer::OnDrawImGui()
    {
        ImGui::ShowDemoWindow();
    }
}
