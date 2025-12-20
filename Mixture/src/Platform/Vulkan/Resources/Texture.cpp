#include "mxpch.hpp"
#include "Platform/Vulkan/Resources/Texture.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    Texture::Texture(const RHI::TextureDesc& spec)
        : m_Width(spec.Width), m_Height(spec.Height), m_Format(spec.Format),
          m_DebugName(spec.DebugName), m_OwnsImage(true)
    {
        // TODO: implement this to use VMA to allocate
        // m_Image and m_Allocation. For now, we leave it empty or
        // call Invalidate() if you are ready.
        Invalidate();
    }

    Texture::Texture(const std::string& path)
        : m_DebugName(path), m_OwnsImage(true)
    {
        // TODO: Load using stb_image, then call Invalidate()
    }

    Texture::Texture(vk::Image image, vk::ImageView imageView, uint32_t width, uint32_t height)
        : m_Image(image), m_ImageView(imageView), m_Width(width), m_Height(height), m_OwnsImage(false)
    {
        // We do not create samplers or allocate memory here.
        // We just act as a handle for the Renderer.
    }

    // -------------------------------------------------------------------------
    // Lifecycle
    // -------------------------------------------------------------------------
    Texture::~Texture()
    {
        Release();
    }

    void Texture::Release()
    {
        // ONLY destroy resources if we allocated them.
        // Swapchain images are owned by the VulkanSwapchain class.
        if (m_OwnsImage)
        {
            auto device = Context::Get().GetLogicalDevice()->GetHandle();
            auto allocator = Context::Get().GetLogicalDevice()->GetAllocator();

            if (m_ImageView) device.destroyImageView(m_ImageView);
            if (m_Sampler) device.destroySampler(m_Sampler);
            if (m_Image && m_Allocation) vmaDestroyImage(allocator, m_Image, m_Allocation);
        }

        // Reset handles
        m_Image = nullptr;
        m_ImageView = nullptr;
        m_Allocation = nullptr;
    }

    // -------------------------------------------------------------------------
    // Invalidate (Actually creates the resources for Case 1)
    // -------------------------------------------------------------------------
    void Texture::Invalidate()
    {
        // If this is a wrapper, we shouldn't be here
        if (!m_OwnsImage) return;

        // Release old resources if resizing/reloading
        Release();

        auto device = Context::Get().GetLogicalDevice();
        auto allocator = device->GetAllocator();

        // Image Info
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM; // Should come from m_Spec.Format
        imageInfo.extent.width = m_Width;
        imageInfo.extent.height = m_Height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        // Allocation Info (VMA)
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

        // Create Image
        // (Cast to C handles for VMA)
        VkImage rawImage;
        if (vmaCreateImage(allocator, &imageInfo, &allocInfo, &rawImage, &m_Allocation, nullptr) != VK_SUCCESS)
        {
            OPAL_ERROR("Core/Vulkan", "Failed to allocate texture image!");
            return;
        }
        m_Image = rawImage;

        // Create View
        vk::ImageViewCreateInfo viewInfo;
        viewInfo.image = m_Image;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = vk::Format::eR8G8B8A8Unorm; // Match image format
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        m_ImageView = device->GetHandle().createImageView(viewInfo);

        // Create Sampler (Optional, but usually needed for textures)
        vk::SamplerCreateInfo samplerInfo;
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = Context::Get().GetPhysicalDevice()->GetProperties().limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;

        m_Sampler = device->GetHandle().createSampler(samplerInfo);
    }

    vk::DescriptorImageInfo Texture::GetDescriptorInfo() const
    {
        vk::DescriptorImageInfo info;
        info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        info.imageView = m_ImageView;
        info.sampler = m_Sampler;
        return info;
    }
}
