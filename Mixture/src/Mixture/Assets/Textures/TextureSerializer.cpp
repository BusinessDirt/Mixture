#include "mxpch.hpp"
#include "Mixture/Assets/Textures/TextureSerializer.hpp"
#include "Mixture/Assets/Textures/TextureAsset.hpp"

#include <stb_image.h>

namespace Mixture
{
    Ref<IAsset> TextureSerializer::Load(FileStreamReader& stream, const AssetMetadata& metadata, ArenaAllocator* allocator)
    {
        // Read file content into buffer
        Vector<char> fileBuffer;
        stream.ReadBuffer(fileBuffer);

        if (fileBuffer.empty())
        {
            OPAL_ERROR("Core/Assets", "Failed to load texture content: {}", metadata.FilePath.string());
            return nullptr;
        }

        int width, height, channels;
        // Force 4 channels (RGBA) for consistency
        stbi_uc* data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(fileBuffer.data()), 
                                              static_cast<int>(fileBuffer.size()), 
                                              &width, &height, &channels, 4); 

        if (!data)
        {
            OPAL_ERROR("Core/Assets", "Failed to decode texture '{}': {}", metadata.FilePath.string(), stbi_failure_reason());
            return nullptr;
        }

        // Copy to vector
        size_t dataSize = width * height * 4;
        Vector<uint8_t> textureData(data, data + dataSize);

        stbi_image_free(data);

        // Name from filename
        std::string name = metadata.FilePath.filename().string();

        return CreateRef<TextureAsset>(metadata.ID, name, static_cast<uint32_t>(width), static_cast<uint32_t>(height), RHI::Format::R8G8B8A8_UNORM, std::move(textureData));
    }
}
