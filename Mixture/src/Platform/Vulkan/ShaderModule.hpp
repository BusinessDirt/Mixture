#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/Shaders/ShaderInformation.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{

    class ShaderModule
    {
    public:
        MX_NON_COPIABLE(ShaderModule);

        ShaderModule(const SPVShader& shader, ShaderStage stage);
        ~ShaderModule();
        
        VkPipelineShaderStageCreateInfo CreateInfo();

    private:
        VULKAN_HANDLE(VkShaderModule, m_ShaderModule);
        VkShaderStageFlagBits m_ShaderStageFlagBits;
    };
}
