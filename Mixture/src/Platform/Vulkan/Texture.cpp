#include "mxpch.hpp"
#include "Platform/Vulkan/Texture.hpp"

#include "Mixture/Renderer/Texture.hpp"

#include "Platform/Vulkan/Buffer/Buffer.hpp"

#include "Mixture/Core/Application.hpp"

#include <stb_image.h>

namespace Mixture::Vulkan
{
    namespace Util
    {
        namespace
        {
            VkFormat MixtureImageFormatToVkFormat(const Jasper::ImageFormat format)
            {
                switch (format)
                {
                    case Jasper::ImageFormat::None: return VK_FORMAT_UNDEFINED;
                    case Jasper::ImageFormat::RGB8:  return VK_FORMAT_R8G8B8_SRGB;
                    case Jasper::ImageFormat::RGBA8: return VK_FORMAT_R8G8B8A8_SRGB;
                    case Jasper::ImageFormat::R8: return VK_FORMAT_R8_SRGB;
                    case Jasper::ImageFormat::RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
                }

                OPAL_CORE_ASSERT(false)
                return VK_FORMAT_UNDEFINED;
            }
		
            uint32_t MixtureImageFormatToByteSize(const Jasper::ImageFormat format)
            {
                switch (format)
                {
                    case Jasper::ImageFormat::None: return 0;
                    case Jasper::ImageFormat::RGB8:  return 3;
                    case Jasper::ImageFormat::RGBA8: return 4;
                    case Jasper::ImageFormat::R8: return 1;
                    case Jasper::ImageFormat::RGBA32F: return 16;
                }

                OPAL_CORE_ASSERT(false)
                return 0;
            }
        }
    }
    
    Texture2D::Texture2D(Jasper::TextureSpecification specification)
        : m_Specification(std::move(specification))
    {
        VkExtent2D extent{ m_Specification.Width, m_Specification.Height };
        VkFormat format = Util::MixtureImageFormatToVkFormat(m_Specification.Format);
        
        m_Image = CreateScope<Image>(extent, format, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, m_Specification.MipLevels);

        m_ImageMemory = CreateScope<DeviceMemory>(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
        m_ImageView = CreateScope<ImageView>(m_Image->GetHandle(), format, VK_IMAGE_ASPECT_COLOR_BIT, m_Specification.MipLevels);
        m_ImageSampler = CreateScope<ImageSampler>();

        m_DescriptorImageInfo.imageLayout = m_Image->GetLayout();
        m_DescriptorImageInfo.imageView = m_ImageView->GetHandle();
        m_DescriptorImageInfo.sampler = m_ImageSampler->GetHandle();
    }

    Texture2D::Texture2D(const std::string& path)
    {
        int width, height, channels;
        stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        OPAL_ASSERT(pixels, "Mixture::Vulkan::Texture2D::Texture2D() - Failed to load image from disk!")

        m_Specification.Width = width;
        m_Specification.Height = height;
        m_Specification.MipLevels = 8;
        m_Specification.Format = Jasper::ImageFormat::RGBA8;
        m_Specification.Path = path;

        VkExtent2D extent{m_Specification.Width, m_Specification.Height};
        const VkDeviceSize imageSize = static_cast<VkDeviceSize>(width) * height * channels;

        Buffer stagingBuffer(imageSize, 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer(pixels, imageSize);
        stagingBuffer.Unmap();
        stbi_image_free(pixels);

        VkFormat format = Util::MixtureImageFormatToVkFormat(m_Specification.Format);
        m_Image = CreateScope<Image>(extent, format, VK_IMAGE_TILING_OPTIMAL,
                                     VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                     m_Specification.MipLevels);

        m_ImageMemory = CreateScope<DeviceMemory>(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
        m_ImageView = CreateScope<ImageView>(m_Image->GetHandle(), format, VK_IMAGE_ASPECT_COLOR_BIT,
                                             m_Specification.MipLevels);

        m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        m_Image->CopyFrom(stagingBuffer);
        m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        m_ImageSampler = CreateScope<ImageSampler>();

        m_DescriptorImageInfo.imageLayout = m_Image->GetLayout();
        m_DescriptorImageInfo.imageView = m_ImageView->GetHandle();
        m_DescriptorImageInfo.sampler = m_ImageSampler->GetHandle();

        m_Loaded = true;
    }

    void Texture2D::SetData(const void* data, const uint32_t size)
    {
        OPAL_CORE_ASSERT(size == m_Specification.Width * m_Specification.Height * Util::MixtureImageFormatToByteSize(m_Specification.Format),
                         "Mixture::Vulkan::Texture2D::SetData() - SetData() requires full texture data!")

        Buffer stagingBuffer(size, 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer(data, size);
        stagingBuffer.Unmap();

        m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        m_Image->CopyFrom(stagingBuffer);
        m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        m_DescriptorImageInfo.imageLayout = m_Image->GetLayout();
    }

    void Texture2D::Bind(uint32_t slot) const
    {
        // No-op in Vulkan. Descriptor set binding happens elsewhere
    }
}
