#pragma once

/**
 * @file InspectorPanel.hpp
 * @brief Panel responsible for inspecting and editing properties of selected scene entities and components.
 */

#include "Panels/IEditorPanel.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Mixture/Scene/Entity.hpp"
#include "Mixture/Scene/Components.hpp"

#include <memory>
#include <string>

namespace Mixture
{
    /**
     * @brief Panel responsible for inspecting and editing entity component properties.
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
            m_HierarchyPanel = std::move(hierarchyPanel);
        }

    private:
        void DrawComponents(Entity entity);
        
        template<typename T, typename UIFunc>
        static void DrawComponentUI(const std::string& name, Entity entity, UIFunc uiFunc);

        static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

    private:
        std::shared_ptr<SceneHierarchyPanel> m_HierarchyPanel;
    };
}
