#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class FrameBuffer;

    class RenderPass
    {
    public:
        MX_NON_COPIABLE(RenderPass);
        
        RenderPass(VkFormat format, bool useDepth = true);
        ~RenderPass();
        
    private:
        VULKAN_HANDLE(VkRenderPass, m_RenderPass);
    };
}
