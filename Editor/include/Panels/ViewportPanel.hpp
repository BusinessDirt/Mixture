#pragma once

#include "Panels/IEditorPanel.hpp"
#include "Mixture/Render/RHI/ITexture.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"
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
        ~ViewportPanel() override;

        void OnDrawImGui() override;
        void OnUpdate(float dt) override;
        void OnEvent(Event& event) override;

        /** Ensures the viewport target texture matches the specified dimensions. */
        void EnsureTextureSize(RHI::IGraphicsDevice& device, uint32_t width, uint32_t height);

        /** Returns the offscreen target texture. */
        OPAL_NODISCARD RHI::ITexture* GetTargetTexture() const { return m_ViewportTexture.get(); }

        /** Returns the size of the viewport rendering area. */
        OPAL_NODISCARD glm::vec2 GetViewportSize() const { return m_ViewportSize; }

        /** Returns whether the viewport window is currently focused. */
        OPAL_NODISCARD bool IsFocused() const { return m_IsFocused; }

        /** Returns whether the cursor is hovering over the viewport. */
        OPAL_NODISCARD bool IsHovered() const { return m_IsHovered; }

    private:
        glm::vec2 m_ViewportSize{ 0.0f, 0.0f };
        glm::vec2 m_ViewportBounds[2]{ { 0.0f, 0.0f }, { 0.0f, 0.0f } };

        Ref<RHI::ITexture> m_ViewportTexture;
        void* m_ViewportTextureID = nullptr;
        uint32_t m_TextureWidth = 0;
        uint32_t m_TextureHeight = 0;

        bool m_IsFocused = false;
        bool m_IsHovered = false;
    };
}
