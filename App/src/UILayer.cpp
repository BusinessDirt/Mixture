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
        const ImGuiIO& io = ImGui::GetIO();
        if (event.IsInCategory(EventCategoryMouse)) event.Handled |= io.WantCaptureMouse;
        if (event.IsInCategory(EventCategoryKeyboard)) event.Handled |= io.WantCaptureKeyboard;
    }

    void UILayer::OnUpdate(float dt)
    {

    }

    void UILayer::OnRender(RenderGraph& graph)
    {
        graph.AddPass<ImGuiPass>("ImGuiPass", Application::Get().GetImGuiContext(),
            graph.GetResource("Backbuffer"));
    }

    void UILayer::OnDrawImGui()
    {
        ImGui::ShowDemoWindow();
    }
}
