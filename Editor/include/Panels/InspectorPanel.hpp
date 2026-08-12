#pragma once

#include "Panels/IEditorPanel.hpp"
#include "Panels/SceneHierarchyPanel.hpp"

namespace Mixture
{
    /**
     * @brief Panel responsible for inspecting and editing properties of selected scene entities.
     */
    class InspectorPanel final : public IEditorPanel
    {
    public:
        explicit InspectorPanel(std::shared_ptr<SceneHierarchyPanel> hierarchyPanel = nullptr);
        ~InspectorPanel() override = default;

        void OnDrawImGui() override;

        /** Sets the target SceneHierarchyPanel to query selection from. */
        void SetHierarchyPanel(std::shared_ptr<SceneHierarchyPanel> hierarchyPanel)
        {
            m_HierarchyPanel = hierarchyPanel;
        }

    private:
        void DrawComponents(EditorEntity& entity);
        static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

    private:
        std::shared_ptr<SceneHierarchyPanel> m_HierarchyPanel;
    };
}
