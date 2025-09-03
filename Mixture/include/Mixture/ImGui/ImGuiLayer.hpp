#pragma once
#include "Mixture/Core/Layer.hpp"

#include "Mixture/Renderer/RendererInfo.hpp"

#include "Platform/Vulkan/Swapchain.hpp"

#include <vulkan/vulkan.h>
#include <imgui.h>

namespace Mixture
{
    class ImGuiLayer final : public Layer
    {
    public:
        OPAL_NON_COPIABLE(ImGuiLayer);
        
        ImGuiLayer();
        ~ImGuiLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnEvent(Event& e) override;

        void OnUpdate(FrameInfo& frameInfo) override {}
        void OnRender(FrameInfo& frameInfo) override {}
        void OnRenderImGui(FrameInfo& frameInfo) override {}

        static void Begin();
        static void End();

        void BlockEvents(const bool block) { m_BlockEvents = block; }

        static void SetDarkThemeColors();

        OPAL_NODISCARD static uint32_t GetActiveWidgetId();

    private:
        bool m_BlockEvents = true;
    };
}
