#include "Panels/ViewportPanel.hpp"
#include "Mixture/Core/Application.hpp"
#include "Mixture/Render/ImGui/Context.hpp"

#include <imgui.h>

namespace Mixture
{
    ViewportPanel::ViewportPanel()
        : IEditorPanel("Viewport", true)
    {
    }

    ViewportPanel::~ViewportPanel()
    {
        if (m_ViewportTextureID && Application::IsCreated())
        {
            Application::Get().GetContext().GetDevice().WaitForIdle();
            Application::Get().GetImGuiContext()->RemoveTextureID(m_ViewportTextureID);
            m_ViewportTextureID = nullptr;
        }
    }

    void ViewportPanel::OnUpdate(float dt)
    {
    }

    void ViewportPanel::OnEvent(Event& event)
    {
    }

    void ViewportPanel::EnsureTextureSize(RHI::IGraphicsDevice& device, uint32_t width, uint32_t height)
    {
        if (width == 0) width = 1;
        if (height == 0) height = 1;

        if (m_TextureWidth != width || m_TextureHeight != height || !m_ViewportTexture)
        {
            device.WaitForIdle();

            if (m_ViewportTextureID && Application::IsCreated())
            {
                Application::Get().GetImGuiContext()->RemoveTextureID(m_ViewportTextureID);
                m_ViewportTextureID = nullptr;
            }

            m_TextureWidth = width;
            m_TextureHeight = height;

            RHI::TextureDesc desc;
            desc.Width = width;
            desc.Height = height;
            desc.PixelFormat = RHI::Format::R8G8B8A8_UNORM;
            desc.Usage = RHI::TextureUsage::ColorAttachment | RHI::TextureUsage::Sampled;
            desc.DebugName = "ViewportTexture";

            m_ViewportTexture = device.CreateTexture(desc);
            if (m_ViewportTexture && Application::IsCreated())
            {
                m_ViewportTextureID = Application::Get().GetImGuiContext()->GetTextureID(m_ViewportTexture.get());
            }
        }
    }

    void ViewportPanel::OnDrawImGui()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
        
        ImGui::Begin(m_Name.c_str(), &m_IsOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        m_IsFocused = ImGui::IsWindowFocused();
        m_IsHovered = ImGui::IsWindowHovered();

        ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
        ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        ImVec2 viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        if (m_ViewportSize.x != viewportPanelSize.x || m_ViewportSize.y != viewportPanelSize.y)
        {
            m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        }

        uint32_t width = static_cast<uint32_t>(m_ViewportSize.x > 0.0f ? m_ViewportSize.x : 1280.0f);
        uint32_t height = static_cast<uint32_t>(m_ViewportSize.y > 0.0f ? m_ViewportSize.y : 720.0f);

        if (Application::IsCreated())
        {
            EnsureTextureSize(Application::Get().GetContext().GetDevice(), width, height);
        }

        if (m_ViewportTextureID)
        {
            ImGui::Image(m_ViewportTextureID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 0), ImVec2(1, 1));
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }
}
