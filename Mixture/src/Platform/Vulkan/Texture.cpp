#include "mxpch.hpp"
#include "Texture.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
    #include <stb_image.h>
#endif

#include "Mixture/Core/Application.hpp"

#include "Platform/Vulkan/Buffer/Buffer.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorSets.hpp"

#include "Platform/Vulkan/Image/Image.hpp"
#include "Platform/Vulkan/Image/ImageView.hpp"
#include "Platform/Vulkan/Image/Sampler.hpp"

#include "Platform/Vulkan/DeviceMemory.hpp"
#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    Texture::Texture(const std::string& filename)
    {
        // use stb_image to laod the texture data
        int texWidth, texHeight, texChannels;
        const std::filesystem::path filePath = Application::Get().GetAssetManager().GetTexturePath(filename);
        stbi_uc* pixels = stbi_load(filePath.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        MX_CORE_ASSERT(pixels, "Failed to load texture image");
        
        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
        
        // upload the data to a staging buffer
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        Buffer stagingBuffer(imageSize, 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer(pixels);
        stagingBuffer.Unmap();
        
        // free the stbi memory
        stbi_image_free(pixels);
        
        // Create the image
        VkExtent2D imageExtent{};
        imageExtent.width = static_cast<uint32_t>(texWidth);
        imageExtent.height = static_cast<uint32_t>(texHeight);
        m_Image = CreateScope<Image>(imageExtent, VK_FORMAT_R8G8B8A8_SRGB, mipLevels);
        
        // Allocate the device memory
        m_ImageMemory = CreateScope<DeviceMemory>(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
        
        // Copy the staging buffer to the image
        m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        m_Image->CopyFrom(stagingBuffer);
        m_Image->GenerateMipMaps(VK_FORMAT_R8G8B8A8_SRGB);
        
        // Create the image view and sampler
        m_ImageView = CreateScope<ImageView>(m_Image->GetHandle(), m_Image->GetFormat(), VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
        
        SamplerConfig samplerConfig{};
        samplerConfig.MaxLod = static_cast<float>(mipLevels);
        m_Sampler = CreateScope<Sampler>(samplerConfig);
    }

    Texture::Texture(uint32_t width, uint32_t height)
    {
        // Create the image
        VkExtent2D imageExtent{};
        imageExtent.width = width;
        imageExtent.height = height;
        m_Image = CreateScope<Image>(imageExtent, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT);

        m_ImageMemory = CreateScope<DeviceMemory>(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

        // Create the image view and sampler
        m_ImageView = CreateScope<ImageView>(m_Image->GetHandle(), m_Image->GetFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
        m_Sampler = CreateScope<Sampler>(SamplerConfig());

        m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    Texture::~Texture()
    {
        m_Sampler = nullptr;
        m_ImageView = nullptr;
        m_Image = nullptr;
        m_ImageMemory = nullptr;
    }
}
