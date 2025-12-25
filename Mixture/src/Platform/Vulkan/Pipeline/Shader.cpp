#include "mxpch.hpp"
#include "Platform/Vulkan/Pipeline/Shader.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    Shader::Shader(const void* data, size_t size, RHI::ShaderStage stage)
        : m_Stage(stage), m_ReflectionData(ShaderCompiler::ReflectSPIRV(data, size))
    {
        vk::ShaderModuleCreateInfo createInfo;
        createInfo.setCodeSize(size);
        createInfo.setPCode(reinterpret_cast<const uint32_t*>(data));

        auto& device = Context::Get().GetLogicalDevice();
        vk::Device vkDevice = device.GetHandle();

        try
        {
            m_Handle = vkDevice.createShaderModule(createInfo);
        }
        catch (const vk::SystemError& e)
        {
            OPAL_ERROR("Core/Vulkan", "Shader creation failed: {}", e.what());
            OPAL_ASSERT("Core", false);
        }
    }

    Shader::~Shader()
    {
        if (m_Handle)
        {
            auto& device = Context::Get().GetLogicalDevice();
            device.GetHandle().destroyShaderModule(m_Handle);
        }
    }

    vk::PipelineShaderStageCreateInfo Shader::CreateInfo()
    {
        vk::ShaderStageFlagBits vkStage = EnumMapper::MapShaderStage(m_Stage);
        vk::PipelineShaderStageCreateInfo createInfo;
        createInfo.setModule(m_Handle);
        createInfo.setStage(vkStage);

        if (m_ReflectionData.EntryPoints.find(m_Stage) != m_ReflectionData.EntryPoints.end())
        {
            createInfo.setPName(m_ReflectionData.EntryPoints[m_Stage].c_str());
        }
        else
        {
            createInfo.setPName("main");
            OPAL_WARN("Core/Vulkan", "Entry point for stage {} not found in reflection data! Defaulting to 'main'.", vkStage);
        }

        return createInfo;
    }
}
