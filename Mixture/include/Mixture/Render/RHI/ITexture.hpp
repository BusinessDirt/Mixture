#pragma once

/**
 * @file ITexture.hpp
 * @brief Interface for GPU textures.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/RHI/RenderFormats.hpp"

namespace Mixture::RHI
{

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
        Format Format = Format::R8G8B8A8_UNORM;

        /**
         * @brief Debug name for the texture.
         */
        const char* DebugName = "Unnamed Texture";
        // MipLevels, IsRenderTarget, etc. can be added later
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
        virtual const char* GetDebugName() const = 0;
    };

}