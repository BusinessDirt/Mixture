#include "mxpch.hpp"
#include "Mixture/Render/ImGui/Context.hpp"
#include "Mixture/Core/Application.hpp"

#include <imgui.h>

namespace Mixture
{
    void ImGuiContext::Initialize()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        //ImGuiStyle& style = ImGui::GetStyle();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        //{
        //    style.WindowRounding = 0.0f;
        //    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        //}
    }

    void ImGuiContext::Shutdown()
    {
        ImGui::DestroyContext();
    }

    void ImGuiContext::BeginFrame()
    {
        Application::Get().GetContext().BeginImGuiFrame();
    }

    void ImGuiContext::EndFrame()
    {
        Application::Get().GetContext().EndImGuiFrame();
    }
}
