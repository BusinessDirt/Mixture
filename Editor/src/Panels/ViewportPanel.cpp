#include "Panels/ViewportPanel.hpp"

#include <imgui.h>

namespace Mixture
{
    ViewportPanel::ViewportPanel()
        : IEditorPanel("Viewport", true)
    {
    }

    void ViewportPanel::OnUpdate(float dt)
    {
    }

    void ViewportPanel::OnEvent(Event& event)
    {
    }

    void ViewportPanel::OnDrawImGui()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
        
        ImGui::Begin(m_Name.c_str(), &m_IsOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        m_IsFocused = ImGui::IsWindowFocused();
        m_IsHovered = ImGui::IsWindowHovered();

        ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
        ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        ImVec2 viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        if (m_ViewportSize.x != viewportPanelSize.x || m_ViewportSize.y != viewportPanelSize.y)
        {
            m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }
}
