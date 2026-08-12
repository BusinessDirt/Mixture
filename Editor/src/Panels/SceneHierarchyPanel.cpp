#include "Panels/SceneHierarchyPanel.hpp"

#include <imgui.h>
#include <algorithm>

namespace Mixture
{
    SceneHierarchyPanel::SceneHierarchyPanel(Ref<Scene> context)
        : IEditorPanel("Scene Hierarchy", true)
    {
        SetContext(context);
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
    {
        m_Context = context;
        ClearSelection();

        // If no context provided, initialize default scene with sample entities
        if (!m_Context)
        {
            m_Context = Scene::Create("Main Scene");
            
            auto camera = m_Context->CreateEntity("Main Camera");
            camera.AddComponent<CameraComponent>();
            camera.GetComponent<TransformComponent>().Position = { 0.0f, 0.0f, 5.0f };

            auto light = m_Context->CreateEntity("Point Light");
            light.AddComponent<LightComponent>();
            light.GetComponent<TransformComponent>().Position = { 2.0f, 4.0f, 2.0f };

            auto cube = m_Context->CreateEntity("Cube");
            cube.AddComponent<MeshRendererComponent>();
        }
    }

    Entity SceneHierarchyPanel::CreateEntity(const std::string& name)
    {
        if (!m_Context)
        {
            SetContext(nullptr);
        }
        return m_Context->CreateEntity(name);
    }

    void SceneHierarchyPanel::DeleteEntity(Entity entity)
    {
        if (m_SelectedEntity == entity)
        {
            ClearSelection();
        }

        if (m_Context && entity.IsValid())
        {
            m_Context->DestroyEntity(entity);
        }
    }

    void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
    {
        m_SelectedEntity = entity;
    }

    void SceneHierarchyPanel::ClearSelection()
    {
        m_SelectedEntity = Entity{};
    }

    void SceneHierarchyPanel::OnDrawImGui()
    {
        ImGui::Begin(m_Name.c_str(), &m_IsOpen);

        if (!m_Context)
        {
            ImGui::TextDisabled("No Active Scene Context");
            ImGui::End();
            return;
        }

        auto rootEntities = m_Context->GetRootEntities();
        for (auto& entity : rootEntities)
        {
            DrawEntityNode(entity);
        }

        // Clear selection when clicking blank space inside window
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        {
            ClearSelection();
        }

        // Right-click context menu on blank space
        if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem("Create Empty Entity"))
            {
                auto newEntity = CreateEntity("Empty Entity");
                SetSelectedEntity(newEntity);
            }
            if (ImGui::MenuItem("Create 3D Object (Cube)"))
            {
                auto cube = CreateEntity("Cube");
                cube.AddComponent<MeshRendererComponent>();
                SetSelectedEntity(cube);
            }
            if (ImGui::MenuItem("Create Point Light"))
            {
                auto light = CreateEntity("Point Light");
                light.AddComponent<LightComponent>();
                SetSelectedEntity(light);
            }
            if (ImGui::MenuItem("Create Camera"))
            {
                auto camera = CreateEntity("Main Camera");
                camera.AddComponent<CameraComponent>();
                SetSelectedEntity(camera);
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        if (!entity.IsValid()) return;

        std::string name = "Entity";
        if (entity.HasComponent<TagComponent>())
        {
            name = entity.GetComponent<TagComponent>().Name;
        }

        ImGuiTreeNodeFlags flags = (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

        auto children = entity.GetChildren();
        if (children.empty())
        {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uintptr_t>(entity.GetID())), flags, "%s", name.c_str());

        if (ImGui::IsItemClicked())
        {
            SetSelectedEntity(entity);
        }

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Create Child Entity"))
            {
                auto child = CreateEntity("Child Entity");
                child.SetParent(entity);
                SetSelectedEntity(child);
            }

            if (entity.HasParent() && ImGui::MenuItem("Unparent"))
            {
                entity.RemoveParent();
            }

            if (ImGui::MenuItem("Delete Entity"))
            {
                entityDeleted = true;
            }
            ImGui::EndPopup();
        }

        if (opened)
        {
            for (auto& child : children)
            {
                DrawEntityNode(child);
            }
            ImGui::TreePop();
        }

        if (entityDeleted)
        {
            DeleteEntity(entity);
        }
    }
}
