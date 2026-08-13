#pragma once
#if defined(OPAL_PLATFORM_DARWIN)

/**
 * @file Texture.hpp
 * @brief Metal implementation of the Texture interface.
 */

#include "Platform/Metal/Definitions.hpp"
#include "Mixture/Render/RHI/ITexture.hpp"

#include <span>

namespace Mixture::Metal
{
    class Device;

    /**
     * @brief Metal implementation of a GPU texture.
     */
    class Texture : public RHI::ITexture
    {
    public:
        Texture(Ref<Device> device, const RHI::TextureDesc& desc, std::span<const std::byte> initialData = {});
        Texture(Ref<Device> device, RHI::Format format, MTL::Texture* texture, uint32_t width, uint32_t height);
        ~Texture();

        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        RHI::Format GetFormat() const override { return m_Format; }
        std::string_view GetDebugName() const override { return m_DebugName; }

        MTL::Texture* GetHandle() const { return m_Texture; }

    private:
        Ref<Device> m_Device;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        RHI::Format m_Format = RHI::Format::R8G8B8A8_UNORM;
        std::string m_DebugName;
        MTL::Texture* m_Texture = nullptr;
        bool m_OwnsTexture = false;
    };
}

#endif
