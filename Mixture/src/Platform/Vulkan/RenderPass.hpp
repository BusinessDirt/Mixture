#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class RenderPass
    {
    public:
        MX_NON_COPIABLE(RenderPass);
        
        RenderPass();
        ~RenderPass();
        
    private:
        VULKAN_HANDLE(VkRenderPass, m_RenderPass);
    };
}
