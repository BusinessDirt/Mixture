#pragma once

/**
 * @file TextureAsset.hpp
 * @brief Asset representation of a CPU-side texture image.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/IAsset.hpp"
#include "Mixture/Render/RHI/RenderFormats.hpp"

namespace Mixture
{
    /**
     * @brief Represents a texture asset containing raw pixel data.
     */
    class TextureAsset : public IAsset
    {
    public:
        /**
         * @brief Constructs a TextureAsset.
         * 
         * @param id The asset ID.
         * @param name The asset name.
         * @param width Width of the texture.
         * @param height Height of the texture.
         * @param format Pixel format of the data.
         * @param data Raw pixel data.
         */
        TextureAsset(UUID id, const std::string& name, uint32_t width, uint32_t height, RHI::Format format, Vector<uint8_t> data)
            : m_ID(id), m_Name(name), m_Width(width), m_Height(height), m_Format(format), m_Data(std::move(data))
        {}

        virtual ~TextureAsset() = default;

        // --- IAsset Interface ---
        UUID GetID() const override { return m_ID; }
        AssetType GetType() const override { return AssetType::Texture; }
        const std::string& GetName() const override { return m_Name; }
        size_t GetMemoryUsage() const override { return sizeof(*this) + m_Data.size() + m_Name.capacity(); }

        // --- Texture Data ---
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        RHI::Format GetFormat() const { return m_Format; }
        const void* GetData() const { return m_Data.data(); }
        size_t GetDataSize() const { return m_Data.size(); }

    private:
        UUID m_ID;
        std::string m_Name;
        uint32_t m_Width;
        uint32_t m_Height;
        RHI::Format m_Format;
        Vector<uint8_t> m_Data;
    };
}
