#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class ShaderCode;

    class VulkanShaderModule
    {
    public:
        MX_NON_COPIABLE(VulkanShaderModule);

        VulkanShaderModule(const ShaderCode& code);
        ~VulkanShaderModule();

    private:
        VULKAN_HANDLE(VkShaderModule, m_ShaderModule);
    };
}
