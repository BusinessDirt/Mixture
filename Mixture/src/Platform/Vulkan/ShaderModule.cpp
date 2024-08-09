#include "mxpch.hpp"
#include "ShaderModule.hpp"

#include "Mixture/Assets/Shaders/ShaderCode.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    ShaderModule::ShaderModule(const ShaderCode& code)
        : m_ShaderStageFlagBits(code.GetStageFlagBits())
    {
        VkShaderModuleCreateInfo createInfo = code.CreateInfo();

        MX_VK_ASSERT(vkCreateShaderModule(Context::Get().Device->GetHandle(), &createInfo, nullptr, &m_ShaderModule),
            "Failed to create VkShaderModule");
    }

    ShaderModule::~ShaderModule()
    {
        if (m_ShaderModule)
        {
            vkDestroyShaderModule(Context::Get().Device->GetHandle(), m_ShaderModule, nullptr);
            m_ShaderModule = nullptr;
        }
    }

    VkPipelineShaderStageCreateInfo ShaderModule::CreateInfo()
    {
        VkPipelineShaderStageCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfo.stage = m_ShaderStageFlagBits;
        createInfo.module = m_ShaderModule;
        createInfo.pName = "main";
        return createInfo;
    }
}
