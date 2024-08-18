#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/ImGui/ImGuiRenderer.hpp"

#include "Platform/Vulkan/RenderPass.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorPool.hpp"
#include "Platform/Vulkan/Buffer/FrameBuffer.hpp"


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
        void BeginFrame(CommandBuffer commandBuffer) override;
        void EndFrame(CommandBuffer commandBuffer) override;

    private:
        Scope<DescriptorPool> m_DescriptorPool = nullptr;
    };
}
