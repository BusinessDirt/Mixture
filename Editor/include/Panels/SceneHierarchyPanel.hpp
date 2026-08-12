#pragma once

/**
 * @file SceneHierarchyPanel.hpp
 * @brief Panel responsible for displaying scene hierarchy, entity tree, and selection state.
 */

#include "Panels/IEditorPanel.hpp"
#include "Mixture/Scene/Scene.hpp"
#include "Mixture/Scene/Entity.hpp"
#include "Mixture/Scene/Components.hpp"

#include <memory>
#include <string>

namespace Mixture
{
    /**
     * @brief Panel displaying entity tree and selection management in the Editor.
     */
    class SceneHierarchyPanel final : public IEditorPanel
    {
    public:
        explicit SceneHierarchyPanel(Ref<Scene> context = nullptr);
        ~SceneHierarchyPanel() override = default;

        void OnDrawImGui() override;

        /** Sets the active scene context for the panel. */
        void SetContext(const Ref<Scene>& context);

        /** Gets the active scene context. */
        OPAL_NODISCARD Ref<Scene> GetContext() const { return m_Context; }

        /** Gets the currently selected entity handle. */
        OPAL_NODISCARD Entity GetSelectedEntity() const { return m_SelectedEntity; }

        /** Sets the selected entity. */
        void SetSelectedEntity(Entity entity);

        /** Clears entity selection. */
        void ClearSelection();

        /** Creates a new entity in the active scene hierarchy. */
        Entity CreateEntity(const std::string& name = "Empty Entity");

        /** Deletes an entity from the active scene. */
        void DeleteEntity(Entity entity);

    private:
        void DrawEntityNode(Entity entity);

    private:
        Ref<Scene> m_Context;
        Entity m_SelectedEntity;
    };
}
