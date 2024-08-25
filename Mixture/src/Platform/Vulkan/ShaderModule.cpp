#include "mxpch.hpp"
#include "ShaderModule.hpp"

#include "Mixture/Assets/Shaders/ShaderInformation.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    ShaderModule::ShaderModule(const SPVShader& shader, ShaderStage stage)
    {
        m_ShaderStageFlagBits = static_cast<VkShaderStageFlagBits>(stage);
        const Vector<uint32_t>& code = shader.Data.at(stage);

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size() * sizeof(uint32_t);
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

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
