#include "mxpch.hpp"
#include "Mixture/ImGui/ImGuiWidgets.hpp"

namespace Mixture
{
    ImGuiID ImGuiWidgets::Dockspace(const std::string_view& windowTitle, const bool fullscreen, const ImGuiWindowFlags windowFlagsOverride,
                            const ImGuiDockNodeFlags dockspaceFlags, bool* pOpen)
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        if (fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);

            windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            // Optional: remove window rounding and border for fullscreen
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        }

        // Apply override flags if provided
        windowFlags |= windowFlagsOverride;

        const ImVec2 oldPadding = ImGui::GetStyle().WindowPadding;
        if (fullscreen) ImGui::GetStyle().WindowPadding = ImVec2(0.0f, 0.0f);

        const std::string nullTerminatedString(windowTitle);
        ImGui::Begin(nullTerminatedString.c_str(), pOpen, windowFlags);

        if (fullscreen)
            ImGui::PopStyleVar(2);

        // Create dockspace
        const ImGuiID dockspaceId = ImGui::GetID("MyDockspace");
        ImGui::DockSpace(dockspaceId, ImVec2(0, 0), dockspaceFlags);
        ImGui::End();
        ImGui::GetStyle().WindowPadding = oldPadding;

        return dockspaceId;
    }
}
