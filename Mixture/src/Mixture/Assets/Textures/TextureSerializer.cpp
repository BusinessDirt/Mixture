#include "mxpch.hpp"
#include "Mixture/Assets/Textures/TextureSerializer.hpp"
#include "Mixture/Assets/Textures/TextureAsset.hpp"

#include <stb_image.h>

namespace Mixture
{
    Ref<IAsset> TextureSerializer::Load(const Vector<char>& data, const AssetMetadata& metadata)
    {
        if (data.empty())
        {
            OPAL_ERROR("Core/Assets", "Failed to load texture content: {}", metadata.FilePath.string());
            return nullptr;
        }

        int width, height, channels;
        // Force 4 channels (RGBA) for consistency
        stbi_uc* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data.data()), 
                                              static_cast<int>(data.size()), 
                                              &width, &height, &channels, 4); 

        if (!pixels)
        {
            OPAL_ERROR("Core/Assets", "Failed to decode texture '{}': {}", metadata.FilePath.string(), stbi_failure_reason());
            return nullptr;
        }

        // Copy to vector
        size_t dataSize = width * height * 4;
        Vector<uint8_t> textureData(pixels, pixels + dataSize);

        stbi_image_free(pixels);

        // Name from filename
        std::string name = metadata.FilePath.filename().string();

        return CreateRef<TextureAsset>(metadata.ID, name, static_cast<uint32_t>(width), static_cast<uint32_t>(height), RHI::Format::R8G8B8A8_UNORM, std::move(textureData));
    }
}
