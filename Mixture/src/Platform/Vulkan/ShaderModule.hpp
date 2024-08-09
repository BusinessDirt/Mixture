#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture 
{
    class ShaderCode;
}

namespace Mixture::Vulkan
{

    class ShaderModule
    {
    public:
        MX_NON_COPIABLE(ShaderModule);

        ShaderModule(const ShaderCode& code);
        ~ShaderModule();
        
        VkPipelineShaderStageCreateInfo CreateInfo();

    private:
        VULKAN_HANDLE(VkShaderModule, m_ShaderModule);
        VkShaderStageFlagBits m_ShaderStageFlagBits;
    };
}
