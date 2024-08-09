#include "mxpch.hpp"
#include "VulkanShaderModule.hpp"

#include "Mixture/Assets/Shaders/ShaderCode.hpp"

#include "Platform/Vulkan/VulkanContext.hpp"

namespace Mixture
{
    VulkanShaderModule::VulkanShaderModule(const ShaderCode& code)
    {
        VkShaderModuleCreateInfo createInfo = code.CreateInfo();

        MX_VK_ASSERT(vkCreateShaderModule(VulkanContext::Get().Device->GetHandle(), &createInfo, nullptr, &m_ShaderModule),
            "Failed to create VkShaderModule");
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        if (m_ShaderModule)
        {
            vkDestroyShaderModule(VulkanContext::Get().Device->GetHandle(), m_ShaderModule, nullptr);
            m_ShaderModule = nullptr;
        }
    }
}
