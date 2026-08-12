#include "Panels/StatsPanel.hpp"

#include <imgui.h>

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

        const auto& stats = RenderStats::Get().GetStats();

        float framerate = stats.FPS > 0.0f ? stats.FPS : ImGui::GetIO().Framerate;
        float frameTime = stats.FrameTimeMs > 0.0f ? stats.FrameTimeMs : (1000.0f / (framerate > 0.0f ? framerate : 60.0f));

        ImGui::Text("Framerate: %.1f FPS", framerate);
        ImGui::Text("Frame Time: %.2f ms", frameTime);

        // Frame Time Graph
        ImGui::PlotLines("##FrameTime", m_FrameTimeHistory.data(), static_cast<int>(m_FrameTimeHistory.size()),
                         static_cast<int>(m_HistoryIndex), "Frame Time (ms)", 0.0f, 33.3f, ImVec2(0, 80));

        ImGui::Separator();

#if defined(OPAL_DIST)
        ImGui::TextDisabled("Performance metrics disabled in Distribution build.");
#else
        if (ImGui::CollapsingHeader("Renderer Stats", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Graphics API: %s", stats.GraphicsAPI.c_str());
            ImGui::Text("Draw Calls: %u", stats.DrawCalls);
            ImGui::Text("Triangles: %u", stats.TriangleCount);
            ImGui::Text("Vertices: %u", stats.VertexCount);
            ImGui::Text("Render Passes: %u", stats.RenderPassCount);
        }

        if (ImGui::CollapsingHeader("Memory", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("VRAM Usage: %.1f MB", stats.VRAMUsageMB);
            ImGui::Text("System RAM: %.1f MB", stats.SystemRAMUsageMB);
        }
#endif

        ImGui::End();
    }
}
