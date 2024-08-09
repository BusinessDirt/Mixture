#include "mxpch.hpp"
#include "ShaderModule.hpp"

#include "Mixture/Assets/Shaders/ShaderCode.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    ShaderModule::ShaderModule(const ShaderCode& code)
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
}
