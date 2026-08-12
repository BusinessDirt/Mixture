#include "Panels/SceneHierarchyPanel.hpp"

#include <imgui.h>
#include <algorithm>

namespace Mixture
{
    SceneHierarchyPanel::SceneHierarchyPanel()
        : IEditorPanel("Scene Hierarchy", true)
    {
        // Populate default sample scene hierarchy
        auto& camera = CreateEntity("Main Camera");
        camera.HasCamera = true;
        camera.HasMeshRenderer = false;
        camera.Position = { 0.0f, 2.0f, 5.0f };

        auto& light = CreateEntity("Directional Light");
        light.HasLight = true;
        light.HasMeshRenderer = false;
        light.Position = { 5.0f, 10.0f, 5.0f };
        light.LightIntensity = 2.5f;

        auto& player = CreateEntity("Player");
        player.Position = { 0.0f, 0.0f, 0.0f };
        player.MaterialColor = { 0.2f, 0.7f, 0.9f, 1.0f };

        auto& ground = CreateEntity("Ground Plane");
        ground.Position = { 0.0f, -1.0f, 0.0f };
        ground.Scale = { 10.0f, 0.1f, 10.0f };
        ground.MaterialColor = { 0.3f, 0.3f, 0.3f, 1.0f };

        m_SelectedEntityID = player.ID;
    }

    EditorEntity& SceneHierarchyPanel::CreateEntity(const std::string& name)
    {
        EditorEntity entity;
        entity.ID = m_NextEntityID++;
        entity.Name = name;
        m_Entities.push_back(entity);
        return m_Entities.back();
    }

    void SceneHierarchyPanel::DeleteEntity(uint32_t id)
    {
        if (m_SelectedEntityID && *m_SelectedEntityID == id)
        {
            m_SelectedEntityID.reset();
        }

        std::erase_if(m_Entities, [id](const EditorEntity& entity) {
            return entity.ID == id;
        });
    }

    EditorEntity* SceneHierarchyPanel::GetSelectedEntity()
    {
        if (!m_SelectedEntityID) return nullptr;
        for (auto& entity : m_Entities)
        {
            if (entity.ID == *m_SelectedEntityID)
                return &entity;
        }
        return nullptr;
    }

    const EditorEntity* SceneHierarchyPanel::GetSelectedEntity() const
    {
        if (!m_SelectedEntityID) return nullptr;
        for (const auto& entity : m_Entities)
        {
            if (entity.ID == *m_SelectedEntityID)
                return &entity;
        }
        return nullptr;
    }

    void SceneHierarchyPanel::SetSelectedEntity(uint32_t id)
    {
        m_SelectedEntityID = id;
    }

    void SceneHierarchyPanel::ClearSelection()
    {
        m_SelectedEntityID.reset();
    }

    void SceneHierarchyPanel::OnDrawImGui()
    {
        ImGui::Begin(m_Name.c_str(), &m_IsOpen);

        for (auto& entity : m_Entities)
        {
            DrawEntityNode(entity);
        }

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        {
            m_SelectedEntityID.reset();
        }

        // Right-click context menu on blank space
        if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem("Create Empty Entity"))
            {
                auto& newEntity = CreateEntity("Empty Entity");
                m_SelectedEntityID = newEntity.ID;
            }
            if (ImGui::MenuItem("Create 3D Object (Cube)"))
            {
                auto& cube = CreateEntity("Cube");
                m_SelectedEntityID = cube.ID;
            }
            if (ImGui::MenuItem("Create Point Light"))
            {
                auto& light = CreateEntity("Point Light");
                light.HasLight = true;
                light.HasMeshRenderer = false;
                m_SelectedEntityID = light.ID;
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(EditorEntity& entity)
    {
        ImGuiTreeNodeFlags flags = ((m_SelectedEntityID && *m_SelectedEntityID == entity.ID) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;

        bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uintptr_t>(entity.ID)), flags, "%s", entity.Name.c_str());

        if (ImGui::IsItemClicked())
        {
            m_SelectedEntityID = entity.ID;
        }

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
            {
                entityDeleted = true;
            }
            ImGui::EndPopup();
        }

        if (opened)
        {
            ImGui::TreePop();
        }

        if (entityDeleted)
        {
            DeleteEntity(entity.ID);
        }
    }
}
