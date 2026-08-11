#include "mxpch.hpp"
#include "Platform/Vulkan/Pipeline/Shader.hpp"

#include "Platform/Vulkan/Device.hpp"

#include <stdexcept>
#include <cstring>

namespace Mixture::Vulkan
{
    Shader::Shader(Ref<Device> device, const void* data, size_t size, RHI::ShaderStage stage,
        RHI::ShaderIdentity identity)
        : m_Device(std::move(device)), m_Stage(stage), m_Identity(identity)
    {
        if (!m_Device) throw std::invalid_argument("Shader requires an owning device");
        if (!m_Identity || m_Identity.Stage != stage)
            throw std::invalid_argument("Shader requires a valid identity for the requested stage");
        if (!data || size < sizeof(uint32_t) || size % sizeof(uint32_t) != 0
            || reinterpret_cast<uintptr_t>(data) % alignof(uint32_t) != 0)
            throw std::invalid_argument("SPIR-V bytecode must be nonempty, word-sized, and uint32-aligned");

        uint32_t magic = 0;
        std::memcpy(&magic, data, sizeof(magic));
        if (magic != 0x07230203u)
            throw std::invalid_argument("Shader bytecode does not contain a SPIR-V header");

        const auto reflector = IShaderReflector::Create(RHI::GraphicsAPI::Vulkan);
        if (!reflector)
            throw std::runtime_error("Vulkan shader reflection is unavailable");
        m_ReflectionData = reflector->Reflect(data, size);

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
            throw std::runtime_error(std::string("Vulkan shader creation failed: ") + e.what());
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
