#pragma once

#include "Panels/IEditorPanel.hpp"
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <memory>
#include <optional>

namespace Mixture
{
    /**
     * @brief Data structure representing an entity node in the editor scene tree.
     */
    struct EditorEntity
    {
        uint32_t ID = 0;
        std::string Name = "Entity";
        bool Active = true;
        uint32_t ParentID = 0;

        // Transform
        glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale{ 1.0f, 1.0f, 1.0f };

        // Material / Renderer properties
        glm::vec4 MaterialColor{ 1.0f, 1.0f, 1.0f, 1.0f };
        float Roughness = 0.5f;
        float Metallic = 0.0f;
        bool HasMeshRenderer = true;

        // Light properties
        bool HasLight = false;
        glm::vec3 LightColor{ 1.0f, 1.0f, 1.0f };
        float LightIntensity = 1.0f;

        // Camera properties
        bool HasCamera = false;
        float Fov = 45.0f;
        float NearClip = 0.1f;
        float FarClip = 1000.0f;
    };

    /**
     * @brief Panel responsible for displaying scene entities, hierarchy tree, and selection context.
     */
    class SceneHierarchyPanel final : public IEditorPanel
    {
    public:
        SceneHierarchyPanel();
        ~SceneHierarchyPanel() override = default;

        void OnDrawImGui() override;

        /** Gets the currently selected entity. */
        OPAL_NODISCARD EditorEntity* GetSelectedEntity();
        OPAL_NODISCARD const EditorEntity* GetSelectedEntity() const;

        /** Sets the selected entity by ID. */
        void SetSelectedEntity(uint32_t id);
        void ClearSelection();

        /** Gets all entities in the scene. */
        OPAL_NODISCARD std::vector<EditorEntity>& GetEntities() { return m_Entities; }
        OPAL_NODISCARD const std::vector<EditorEntity>& GetEntities() const { return m_Entities; }

        /** Creates a new entity in the scene hierarchy. */
        EditorEntity& CreateEntity(const std::string& name = "Empty Entity");

        /** Deletes an entity by ID. */
        void DeleteEntity(uint32_t id);

    private:
        void DrawEntityNode(EditorEntity& entity);

    private:
        std::vector<EditorEntity> m_Entities;
        std::optional<uint32_t> m_SelectedEntityID;
        uint32_t m_NextEntityID = 1;
    };
}
