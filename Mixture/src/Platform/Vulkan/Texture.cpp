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
    Texture::Texture(const std::string& filename, const SampledImageInformation& sampler)
    {
        // use stb_image to laod the texture data
        int texWidth, texHeight, texChannels;
        const std::filesystem::path filePath = Application::Get().GetAssetManager().GetTexturePath(filename);
        stbi_uc* pixels = stbi_load(filePath.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        MX_CORE_ASSERT(pixels, "Failed to load texture image");
        
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
        m_Image = CreateScope<Image>(imageExtent, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        
        // Allocate the device memory
        m_ImageMemory = CreateScope<DeviceMemory>(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
        
        // Copy the staging buffer to the image
        m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        m_Image->CopyFrom(stagingBuffer);
        m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        // Create the image view and sampler
        m_ImageView = CreateScope<ImageView>(m_Image->GetHandle(), m_Image->GetFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
        m_Sampler = CreateScope<Sampler>(SamplerConfig());
        
        // TODO: might need to change this implementation when using other descriptor types as well
        DescriptorSets& descriptorSets = Context::Get().DescriptorSetManager->GetSets();
        for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = m_ImageView->GetHandle();
            imageInfo.sampler = m_Sampler->GetHandle();
            
            const std::vector<VkWriteDescriptorSet> descriptorWrites =
            {
                descriptorSets.Bind(i, sampler.Binding, imageInfo)
            };

            descriptorSets.Update(i, descriptorWrites);
        }
    }

    Texture::~Texture()
    {
        m_Sampler = nullptr;
        m_ImageView = nullptr;
        m_Image = nullptr;
        m_ImageMemory = nullptr;
    }
}