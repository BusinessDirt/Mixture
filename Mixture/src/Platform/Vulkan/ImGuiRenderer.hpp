#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/ImGui/ImGuiRenderer.hpp"

#include "Platform/Vulkan/Descriptor/DescriptorPool.hpp"
#include "Platform/Vulkan/RenderPass.hpp"
#include "Platform/Vulkan/Buffer/FrameBuffer.hpp"
#include "Platform/Vulkan/Texture.hpp"


#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class ImGuiRenderer : public Mixture::ImGuiRenderer
    {
    public:
        MX_NON_COPIABLE(ImGuiRenderer);
        
        ImGuiRenderer();
        ~ImGuiRenderer() override;

        void Init() override;
        void Begin() override;
        void End() override;
        void Render(CommandBuffer commandBuffer) override;
        void OnWindowResize(uint32_t width, uint32_t height) override;
        void* GetViewportImage() override;

    private:
        Scope<DescriptorPool> m_DescriptorPool = nullptr;
        Scope<RenderPass> m_RenderPass = nullptr;
        std::vector<Scope<FrameBuffer>> m_FrameBuffers;
    };
}
