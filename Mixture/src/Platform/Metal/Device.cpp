#if defined(OPAL_PLATFORM_DARWIN)
#include "mxpch.hpp"
#include "Platform/Metal/Device.hpp"
#include "Platform/Metal/Pipeline/Shader.hpp"
#include "Platform/Metal/Resources/Buffer.hpp"
#include "Platform/Metal/Resources/Texture.hpp"
#include "Platform/Metal/Pipeline/Pipeline.hpp"

namespace Mixture::Metal
{
	Device::Device()
	{
        m_Device = MTL::CreateSystemDefaultDevice();
        if (!m_Device)
        {
            OPAL_ERROR("Core/Metal", "Failed to create the system Metal device.");
            return;
        }

        OPAL_INFO("Core/Metal", "Using Metal device: {}", m_Device->name()->utf8String());
        m_CommandQueue = m_Device->newCommandQueue();
        if (!m_CommandQueue)
            OPAL_ERROR("Core/Metal", "Failed to create a Metal command queue.");
	}

	Device::~Device()
	{
        if (m_CommandQueue)
        {
            m_CommandQueue->release();
            m_CommandQueue = nullptr;
        }
        if (m_Device)
        {
            m_Device->release();
            m_Device = nullptr;
        }
	}

    Ref<RHI::IShader> Device::CreateShader(const void* data, size_t size, RHI::ShaderStage stage,
        RHI::ShaderIdentity identity, const ShaderReflectionData& reflection)
    {
        OPAL_LOG_DEBUG("Core/Metal", "Creating {} shader ({} bytes).", static_cast<uint32_t>(stage), size);
        return CreateRef<Shader>(shared_from_this(), data, size, stage, identity, reflection);
    }

    Ref<RHI::IBuffer> Device::CreateBuffer(const RHI::BufferDesc& desc, std::span<const std::byte> initialData)
    {
        OPAL_LOG_DEBUG("Core/Metal", "Creating {} buffer '{}' ({} bytes).", static_cast<uint32_t>(desc.Usage), desc.DebugName, desc.Size);
        if (!RHI::IsBufferUploadValid(desc, initialData))
        {
            OPAL_ERROR("Core/Metal", "Rejected buffer '{}' with invalid size or initial-data length", desc.DebugName);
            return nullptr;
        }
        return CreateRef<Buffer>(shared_from_this(), desc, initialData);
    }

    Ref<RHI::ITexture> Device::CreateTexture(const RHI::TextureDesc& desc, std::span<const std::byte> initialData)
    {
        OPAL_LOG_DEBUG("Core/Metal", "Creating texture '{}' ({}x{}).", desc.DebugName, desc.Width, desc.Height);
        if (!RHI::IsTextureUploadValid(desc, initialData))
        {
            OPAL_ERROR("Core/Metal", "Rejected texture '{}' with invalid dimensions, format, or initial-data length", desc.DebugName);
            return nullptr;
        }
        return CreateRef<Texture>(shared_from_this(), desc, initialData);
    }

    Ref<RHI::IPipeline> Device::CreatePipeline(const RHI::PipelineDesc& desc)
    {
        OPAL_LOG_DEBUG("Core/Metal", "Creating render pipeline '{}'.", desc.DebugName);
        return CreateRef<Pipeline>(shared_from_this(), desc);
    }
}

#endif
