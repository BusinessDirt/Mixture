#pragma once

/**
 * @file ITexture.hpp
 * @brief Interface for GPU textures.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/RenderFormats.hpp"
#include "Mixture/Render/RHI/ResourceStates.hpp"

#include <string_view>

namespace Mixture::RHI
{
    enum class TextureUsage : uint32_t
    {
        None = 0,
        Sampled = 1u << 0,
        Storage = 1u << 1,
        ColorAttachment = 1u << 2,
        DepthStencilAttachment = 1u << 3,
        TransferSource = 1u << 4,
        TransferDestination = 1u << 5
    };

    inline TextureUsage operator|(TextureUsage lhs, TextureUsage rhs)
    {
        return static_cast<TextureUsage>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }
    inline TextureUsage& operator|=(TextureUsage& lhs, TextureUsage rhs) { return lhs = lhs | rhs; }
    inline bool HasUsage(TextureUsage value, TextureUsage flag)
    {
        return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
    }

    /**
     * @brief Descriptor structure used to create a texture.
     */
    struct TextureDesc
    {
        /**
         * @brief The width of the texture in pixels.
         */
        uint32_t Width = 1;

        /**
         * @brief The height of the texture in pixels.
         */
        uint32_t Height = 1;

        /**
         * @brief The pixel format of the texture.
         */
        Format PixelFormat = Format::R8G8B8A8_UNORM;

        RHI::ResourceState InitialState = RHI::ResourceState::Undefined;
        TextureUsage Usage = TextureUsage::Sampled | TextureUsage::TransferDestination;

        /**
         * @brief Debug name for the texture.
         */
        std::string_view DebugName = "Unnamed Texture";
        // MipLevels, IsRenderTarget, etc. can be added later

        bool operator==(const TextureDesc& other) const
        {
            return Width == other.Width &&
                   Height == other.Height &&
                   PixelFormat == other.PixelFormat &&
                   InitialState == other.InitialState &&
                   Usage == other.Usage;
        }
    };

    /**
     * @brief Interface representing a GPU texture.
     */
    class ITexture
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~ITexture() = default;

        /**
         * @brief Retrieves the width of the texture.
         * @return The width in pixels.
         */
        virtual uint32_t GetWidth() const = 0;

        /**
         * @brief Retrieves the height of the texture.
         * @return The height in pixels.
         */
        virtual uint32_t GetHeight() const = 0;

        /**
         * @brief Retrieves the pixel format of the texture.
         * @return The Format enum value.
         */
        virtual Format GetFormat() const = 0;

        /**
         * @brief Retrieves the debug name of the texture.
         * @return A C-string representing the debug name.
         */
        virtual std::string_view GetDebugName() const = 0;
    };

}
