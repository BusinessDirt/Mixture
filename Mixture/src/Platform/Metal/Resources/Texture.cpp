#if defined(OPAL_PLATFORM_DARWIN)
#include "mxpch.hpp"
#include "Platform/Metal/Resources/Texture.hpp"
#include "Platform/Metal/Device.hpp"
#include "Platform/Metal/EnumMapper.hpp"

namespace Mixture::Metal
{
    Texture::Texture(Ref<Device> device, const RHI::TextureDesc& desc, std::span<const std::byte> initialData)
        : m_Device(device), m_Width(desc.Width), m_Height(desc.Height), m_Format(desc.PixelFormat), m_DebugName(desc.DebugName), m_OwnsTexture(true)
    {
        if (!m_Device || !m_Device->GetHandle())
        {
            OPAL_ERROR("Core/Metal", "Cannot create texture '{}' without a Metal device.", desc.DebugName);
            return;
        }
        MTL::PixelFormat mtlFormat = EnumMapper::MapFormat(desc.PixelFormat);
        MTL::TextureDescriptor* texDesc = MTL::TextureDescriptor::texture2DDescriptor(mtlFormat, desc.Width, desc.Height, false);
        texDesc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
        m_Texture = m_Device->GetHandle()->newTexture(texDesc);
        if (!m_Texture)
        {
            OPAL_ERROR("Core/Metal", "Failed to allocate texture '{}' ({}x{}).", desc.DebugName, desc.Width, desc.Height);
            return;
        }

        if (!initialData.empty() && m_Texture)
        {
            MTL::Region region = MTL::Region(0, 0, desc.Width, desc.Height);
            uint32_t bytesPerRow = desc.Width * GetFormatStride(desc.PixelFormat);
            m_Texture->replaceRegion(region, 0, initialData.data(), bytesPerRow);
        }
    }

    Texture::Texture(Ref<Device> device, RHI::Format format, MTL::Texture* texture, uint32_t width, uint32_t height)
        : m_Device(device), m_Width(width), m_Height(height), m_Format(format), m_Texture(texture), m_OwnsTexture(false)
    {
    }

    Texture::~Texture()
    {
        if (m_OwnsTexture && m_Texture)
        {
            m_Texture->release();
            m_Texture = nullptr;
        }
    }
}
#endif
