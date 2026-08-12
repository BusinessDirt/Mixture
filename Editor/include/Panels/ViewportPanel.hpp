#pragma once

#include "Panels/IEditorPanel.hpp"
#include <glm/glm.hpp>

namespace Mixture
{
    /**
     * @brief Panel responsible for displaying the main 3D/2D game viewport.
     */
    class ViewportPanel final : public IEditorPanel
    {
    public:
        ViewportPanel();
        ~ViewportPanel() override = default;

        void OnDrawImGui() override;
        void OnUpdate(float dt) override;
        void OnEvent(Event& event) override;

        /** Returns the size of the viewport rendering area. */
        OPAL_NODISCARD glm::vec2 GetViewportSize() const { return m_ViewportSize; }

        /** Returns whether the viewport window is currently focused. */
        OPAL_NODISCARD bool IsFocused() const { return m_IsFocused; }

        /** Returns whether the cursor is hovering over the viewport. */
        OPAL_NODISCARD bool IsHovered() const { return m_IsHovered; }

    private:
        glm::vec2 m_ViewportSize{ 0.0f, 0.0f };
        glm::vec2 m_ViewportBounds[2]{ { 0.0f, 0.0f }, { 0.0f, 0.0f } };

        bool m_IsFocused = false;
        bool m_IsHovered = false;
    };
}
