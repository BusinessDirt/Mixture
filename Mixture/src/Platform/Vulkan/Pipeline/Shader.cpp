#include "mxpch.hpp"
#include "Platform/Vulkan/Pipeline/Shader.hpp"

#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    Shader::Shader(Ref<Device> device, const void* data, size_t size, RHI::ShaderStage stage,
        RHI::ShaderIdentity identity)
        : m_Device(std::move(device)), m_Stage(stage), m_Identity(identity),
          m_ReflectionData(ShaderCompiler::ReflectSPIRV(data, size))
    {
        OPAL_ASSERT("Core/Vulkan", m_Device, "Shader requires an owning device");
        OPAL_ASSERT("Core/Vulkan", m_Identity && m_Identity.Stage == stage,
            "Shader requires a valid identity for the requested stage");

        vk::ShaderModuleCreateInfo createInfo;
        createInfo.setCodeSize(size);
        createInfo.setPCode(reinterpret_cast<const uint32_t*>(data));

        vk::Device vkDevice = m_Device->GetHandle();

        try
        {
            m_Handle = vkDevice.createShaderModule(createInfo);
        }
        catch (const vk::SystemError& e)
        {
            OPAL_ERROR("Core/Vulkan", "Shader creation failed: {}", e.what());
            OPAL_ASSERT("Core", false);
        }

        OPAL_LOG_DEBUG("Core/Vulkan", "Created Shader: stage='{}' size={}", EnumMapper::MapShaderStage(m_Stage), size);
    }

    Shader::~Shader()
    {
        if (m_Handle)
        {
            m_Device->GetHandle().destroyShaderModule(m_Handle);
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
