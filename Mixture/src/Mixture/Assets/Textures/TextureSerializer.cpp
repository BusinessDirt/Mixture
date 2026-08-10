#include "mxpch.hpp"
#include "Mixture/Assets/Textures/TextureSerializer.hpp"
#include "Mixture/Assets/Textures/TextureAsset.hpp"

#include <stb_image.h>
#include <limits>

namespace Mixture
{
    Ref<IAsset> TextureSerializer::Load(const Vector<char>& data, const AssetMetadata& metadata)
    {
        if (data.empty())
        {
            OPAL_ERROR("AssetManager", "Failed to load texture content: {}", metadata.FilePath.string());
            return nullptr;
        }

        if (data.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            OPAL_ERROR("AssetManager", "Texture file is too large for the decoder: {}", metadata.FilePath.string());
            return nullptr;
        }

        int width, height, channels;
        // Force 4 channels (RGBA) for consistency
        stbi_uc* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data.data()),
                                              static_cast<int>(data.size()),
                                              &width, &height, &channels, 4);

        if (!pixels)
        {
            OPAL_ERROR("AssetManager", "Failed to decode texture '{}': {}", metadata.FilePath.string(), stbi_failure_reason());
            return nullptr;
        }

        if (width <= 0 || height <= 0
            || static_cast<size_t>(width) > std::numeric_limits<size_t>::max() / static_cast<size_t>(height)
            || static_cast<size_t>(width) * static_cast<size_t>(height) > std::numeric_limits<size_t>::max() / 4)
        {
            stbi_image_free(pixels);
            OPAL_ERROR("AssetManager", "Decoded texture dimensions overflow: {}", metadata.FilePath.string());
            return nullptr;
        }

        // Copy to vector
        const size_t dataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
        Vector<uint8_t> textureData(pixels, pixels + dataSize);

        stbi_image_free(pixels);

        // Name from filename
        std::string name = metadata.FilePath.filename().string();

        return CreateRef<TextureAsset>(metadata.ID, name, static_cast<uint32_t>(width), static_cast<uint32_t>(height), RHI::Format::R8G8B8A8_UNORM, std::move(textureData));
    }
}
