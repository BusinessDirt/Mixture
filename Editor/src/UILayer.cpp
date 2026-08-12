#include "UILayer.hpp"
#include "Panels/ViewportPanel.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/InspectorPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"
#include "Panels/ConsolePanel.hpp"
#include "Panels/StatsPanel.hpp"

#include <imgui_internal.h>
#include <algorithm>

namespace Mixture
{
    void UILayer::OnAttach()
    {
        OPAL_INFO("Client", "UILayer::OnAttach()");
        m_IsAttached = true;

        if (m_Panels.empty())
        {
            auto hierarchy = RegisterPanel<SceneHierarchyPanel>();
            RegisterPanel<InspectorPanel>(hierarchy);
            RegisterPanel<ViewportPanel>();
            RegisterPanel<ContentBrowserPanel>();
            RegisterPanel<ConsolePanel>();
            RegisterPanel<StatsPanel>();
        }

        for (auto& panel : m_Panels)
        {
            if (panel)
            {
                panel->OnAttach();
            }
        }
    }

    void UILayer::OnDetach()
    {
        OPAL_INFO("Client", "UILayer::OnDetach()");
        for (auto& panel : m_Panels)
        {
            if (panel)
            {
                panel->OnDetach();
            }
        }

        m_IsAttached = false;
    }

    void UILayer::RegisterPanel(const Ref<IEditorPanel>& panel)
    {
        if (!panel) return;

        auto it = std::find(m_Panels.begin(), m_Panels.end(), panel);
        if (it == m_Panels.end())
        {
            m_Panels.push_back(panel);
            if (m_IsAttached)
            {
                panel->OnAttach();
            }
        }
    }

    void UILayer::UnregisterPanel(const Ref<IEditorPanel>& panel)
    {
        if (!panel) return;

        auto it = std::find(m_Panels.begin(), m_Panels.end(), panel);
        if (it != m_Panels.end())
        {
            if (m_IsAttached)
            {
                panel->OnDetach();
            }
            m_Panels.erase(it);
        }
    }

    void UILayer::OnEvent(Event& event)
    {
        for (auto& panel : m_Panels)
        {
            if (panel && panel->IsOpen())
            {
                panel->OnEvent(event);
                if (event.Handled)
                    break;
            }
        }

        const ImGuiIO& io = ImGui::GetIO();
        if (event.IsInCategory(EventCategoryMouse)) event.Handled |= io.WantCaptureMouse;
        if (event.IsInCategory(EventCategoryKeyboard)) event.Handled |= io.WantCaptureKeyboard;
    }

    void UILayer::OnUpdate(float dt)
    {
        for (auto& panel : m_Panels)
        {
            if (panel && panel->IsOpen())
            {
                panel->OnUpdate(dt);
            }
        }
    }

    void UILayer::OnRender(RenderGraph& graph)
    {
        graph.AddPass<ImGuiPass>("ImGuiPass", Application::Get().GetImGuiContext(), graph.GetResource("Backbuffer"));
    }

    void UILayer::SetupDefaultDockLayout(ImGuiID dockspaceId)
    {
        ImGui::DockBuilderRemoveNode(dockspaceId);
        ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);

        ImGuiID dockMainId = dockspaceId;
        ImGuiID dockLeftId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Left, 0.20f, nullptr, &dockMainId);
        ImGuiID dockRightId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Right, 0.25f, nullptr, &dockMainId);
        ImGuiID dockBottomId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Down, 0.30f, nullptr, &dockMainId);

        ImGui::DockBuilderDockWindow("Viewport", dockMainId);
        ImGui::DockBuilderDockWindow("Scene Hierarchy", dockLeftId);
        ImGui::DockBuilderDockWindow("Inspector", dockRightId);
        ImGui::DockBuilderDockWindow("Content Browser", dockBottomId);
        ImGui::DockBuilderDockWindow("Console", dockBottomId);
        ImGui::DockBuilderDockWindow("Performance Stats", dockBottomId);

        ImGui::DockBuilderFinish(dockspaceId);
    }

    void UILayer::OnDrawImGui()
    {
        static constexpr bool opt_fullscreen = true;
        static constexpr bool opt_padding = false;
        static constexpr ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        if constexpr (!opt_padding)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        }

        ImGui::Begin("DockSpace", nullptr, window_flags);

        if constexpr (!opt_padding)
        {
            ImGui::PopStyleVar();
        }

        if constexpr (opt_fullscreen)
        {
            ImGui::PopStyleVar(2);
        }

        // Submit the DockSpace
        const ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            const ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");

            // Build default layout if uninitialized or reset requested
            if (!ImGui::DockBuilderGetNode(dockspace_id) || m_ResetLayoutRequested)
            {
                m_ResetLayoutRequested = false;
                SetupDefaultDockLayout(dockspace_id);
            }

            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        // Render main menu bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                {
                    Application::Get().Close();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Reset Layout"))
                {
                    m_ResetLayoutRequested = true;
                }

                if (ImGui::BeginMenu("Theme"))
                {
                    const auto& themes = ThemeManager::Get().GetThemes();
                    auto activeTheme = ThemeManager::Get().GetActiveTheme();
                    std::string activeName = activeTheme ? activeTheme->GetName() : "";

                    for (const auto& theme : themes)
                    {
                        if (theme)
                        {
                            bool isSelected = (theme->GetName() == activeName);
                            if (ImGui::MenuItem(theme->GetName().c_str(), nullptr, isSelected))
                            {
                                ThemeManager::Get().SetTheme(theme);
                            }
                        }
                    }
                    ImGui::EndMenu();
                }

                if (!m_Panels.empty())
                {
                    ImGui::Separator();
                    for (auto& panel : m_Panels)
                    {
                        if (panel)
                        {
                            bool open = panel->IsOpen();
                            if (ImGui::MenuItem(panel->GetName().c_str(), nullptr, &open))
                            {
                                panel->SetOpen(open);
                            }
                        }
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Render registered panels
        for (auto& panel : m_Panels)
        {
            if (panel && panel->IsOpen())
            {
                panel->OnDrawImGui();
            }
        }

        ImGui::End();
    }
}
