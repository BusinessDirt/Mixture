#if defined(OPAL_PLATFORM_DARWIN)
#include "mxpch.hpp"
#include "Platform/Metal/Resources/Buffer.hpp"
#include "Platform/Metal/Device.hpp"

namespace Mixture::Metal
{
    Buffer::Buffer(Ref<Device> device, const RHI::BufferDesc& desc, std::span<const std::byte> initialData)
        : m_Device(device), m_Desc(desc)
    {
        MTL::ResourceOptions options = MTL::ResourceStorageModeShared;
        if (!initialData.empty())
        {
            m_Buffer = m_Device->GetHandle()->newBuffer(initialData.data(), desc.Size, options);
        }
        else
        {
            m_Buffer = m_Device->GetHandle()->newBuffer(desc.Size, options);
        }
    }

    Buffer::~Buffer()
    {
        if (m_Buffer)
        {
            m_Buffer->release();
            m_Buffer = nullptr;
        }
    }
}
#endif
