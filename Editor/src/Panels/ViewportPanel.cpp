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

        DrawToolbar();

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

        // Draw Viewport Information Overlay
        ImVec2 overlayPos = ImVec2(m_ViewportBounds[0].x + 10.0f, m_ViewportBounds[0].y + 40.0f);
        ImGui::SetNextWindowPos(overlayPos);
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGuiWindowFlags overlayFlags = ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                                        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

        if (ImGui::BeginChild("ViewportOverlay", ImVec2(0, 0), ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding, overlayFlags))
        {
            ImGui::Text("Resolution: %.0f x %.0f", m_ViewportSize.x, m_ViewportSize.y);
            ImGui::Text("FPS: %.1f (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        }
        ImGui::EndChild();

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void ViewportPanel::DrawToolbar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 4.0f));

        if (ImGui::BeginChild("ViewportToolbar", ImVec2(0, 36), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar))
        {
            // Transform Gizmo Mode Toggles
            if (ImGui::RadioButton("Translate", m_GizmoType == 0)) m_GizmoType = 0;
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", m_GizmoType == 1)) m_GizmoType = 1;
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", m_GizmoType == 2)) m_GizmoType = 2;

            ImGui::SameLine(0.0f, 30.0f);

            // Simulation Controls
            const char* playText = m_IsPlaying ? "Stop" : "Play";
            if (ImGui::Button(playText, ImVec2(60, 0)))
            {
                m_IsPlaying = !m_IsPlaying;
                if (!m_IsPlaying) m_IsPaused = false;
            }

            ImGui::SameLine();
            if (m_IsPlaying)
            {
                const char* pauseText = m_IsPaused ? "Resume" : "Pause";
                if (ImGui::Button(pauseText, ImVec2(60, 0)))
                {
                    m_IsPaused = !m_IsPaused;
                }
            }
        }
        ImGui::EndChild();

        ImGui::PopStyleVar(2);
    }
}
