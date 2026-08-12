#include "Panels/StatsPanel.hpp"

#include <imgui.h>
#include <numeric>

namespace Mixture
{
    StatsPanel::StatsPanel()
        : IEditorPanel("Performance Stats", true)
    {
        m_FrameTimeHistory.fill(16.6f);
    }

    void StatsPanel::OnUpdate(float dt)
    {
        m_FrameTimeHistory[m_HistoryIndex] = dt * 1000.0f; // in milliseconds
        m_HistoryIndex = (m_HistoryIndex + 1) % m_FrameTimeHistory.size();
    }

    void StatsPanel::OnDrawImGui()
    {
        ImGui::Begin(m_Name.c_str(), &m_IsOpen);

        float framerate = ImGui::GetIO().Framerate;
        float frameTime = 1000.0f / (framerate > 0.0f ? framerate : 60.0f);

        ImGui::Text("Framerate: %.1f FPS", framerate);
        ImGui::Text("Frame Time: %.2f ms", frameTime);

        // Frame Time Graph
        ImGui::PlotLines("##FrameTime", m_FrameTimeHistory.data(), static_cast<int>(m_FrameTimeHistory.size()),
                         static_cast<int>(m_HistoryIndex), "Frame Time (ms)", 0.0f, 33.3f, ImVec2(0, 80));

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Renderer Stats", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Graphics API: Vulkan 1.3");
            ImGui::Text("Draw Calls: 12");
            ImGui::Text("Triangles: 14,520");
            ImGui::Text("Vertices: 28,900");
            ImGui::Text("Render Passes: 2 (GBuffer, ImGuiPass)");
        }

        if (ImGui::CollapsingHeader("Memory", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("VRAM Usage: 142.5 MB");
            ImGui::Text("System RAM: 38.2 MB");
        }

        ImGui::End();
    }
}
