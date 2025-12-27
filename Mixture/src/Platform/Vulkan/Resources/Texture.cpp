#include "mxpch.hpp"
#include "Platform/Vulkan/Resources/Texture.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Command/Pool.hpp"
#include "Platform/Vulkan/SingleTimeCommand.hpp"

namespace Mixture::Vulkan
{
    Texture::Texture(const RHI::TextureDesc& spec, const void* data)
        : m_Width(spec.Width), m_Height(spec.Height), m_Format(spec.Format),
          m_DebugName(spec.DebugName), m_OwnsImage(true)
    {
        Invalidate();

        if (data)
        {
            VkDeviceSize imageSize = m_Width * m_Height * RHI::GetFormatStride(m_Format);

            auto& logicalDevice = Context::Get().GetLogicalDevice();
            auto allocator = logicalDevice.GetAllocator();

            // Staging Buffer
            VkBufferCreateInfo bufferInfo = {};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = imageSize;
            bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

            VkBuffer stagingBuffer;
            VmaAllocation stagingAllocation;
            VmaAllocationInfo stagingAllocInfo;

            if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &stagingBuffer, &stagingAllocation, &stagingAllocInfo) != VK_SUCCESS)
            {
                OPAL_ERROR("Core/Vulkan", "Failed to create staging buffer for texture: {0}", m_DebugName);
                return;
            }

            // Copy data to staging buffer
            memcpy(stagingAllocInfo.pMappedData, data, static_cast<size_t>(imageSize));

            // Upload to Image
            SingleTimeCommand::Submit(logicalDevice.GetTransferQueue(), Context::Get().GetTransferCommandPool().GetHandle(),
            [&](vk::CommandBuffer cmd)
            {
                vk::ImageMemoryBarrier barrier{};
                barrier.oldLayout = vk::ImageLayout::eUndefined;
                barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.image = m_Image;
                barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
                barrier.subresourceRange.baseMipLevel = 0;
                barrier.subresourceRange.levelCount = 1;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount = 1;
                barrier.srcAccessMask = vk::AccessFlagBits::eNone;
                barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

                cmd.pipelineBarrier(
                    vk::PipelineStageFlagBits::eTopOfPipe,
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::DependencyFlags(),
                    0, nullptr,
                    0, nullptr,
                    1, &barrier
                );

                vk::BufferImageCopy region{};
                region.bufferOffset = 0;
                region.bufferRowLength = 0;
                region.bufferImageHeight = 0;
                region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                region.imageSubresource.mipLevel = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount = 1;
                region.imageOffset = vk::Offset3D{0, 0, 0};
                region.imageExtent = vk::Extent3D{static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height), 1};

                cmd.copyBufferToImage(vk::Buffer(stagingBuffer), m_Image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

                barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
                barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
                barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

                cmd.pipelineBarrier(
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::PipelineStageFlagBits::eFragmentShader,
                    vk::DependencyFlags(),
                    0, nullptr,
                    0, nullptr,
                    1, &barrier
                );
            });

            vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
        }
    }

    Texture::Texture(vk::Format format, vk::Image image, vk::ImageView imageView, uint32_t width, uint32_t height)
        : m_Format(EnumMapper::MapFormat(format)), m_Image(image)
        , m_ImageView(imageView), m_Width(width), m_Height(height)
        , m_OwnsImage(false)
    {}

    Texture::~Texture()
    {
        Release();
    }

    void Texture::Release()
    {
        if (m_OwnsImage)
        {
            auto device = Context::Get().GetLogicalDevice().GetHandle();
            auto allocator = Context::Get().GetLogicalDevice().GetAllocator();

            if (m_ImageView) device.destroyImageView(m_ImageView);
            if (m_Sampler) device.destroySampler(m_Sampler);
            if (m_Image && m_Allocation) vmaDestroyImage(allocator, m_Image, m_Allocation);
        }

        // Reset handles
        m_Image = nullptr;
        m_ImageView = nullptr;
        m_Allocation = nullptr;
    }

    void Texture::Invalidate()
    {
        // If this is a wrapper, we shouldn't be here
        if (!m_OwnsImage) return;

        // Release old resources if resizing/reloading
        Release();

        auto device = Context::Get().GetLogicalDevice();
        auto allocator = device.GetAllocator();

        // Image Info
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = static_cast<VkFormat>(EnumMapper::MapFormat(m_Format));
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
        viewInfo.format = vk::Format(imageInfo.format);
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        m_ImageView = device.GetHandle().createImageView(viewInfo);

        // Create Sampler (Optional, but usually needed for textures)
        vk::SamplerCreateInfo samplerInfo;
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = Context::Get().GetPhysicalDevice().GetProperties().limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;

        m_Sampler = device.GetHandle().createSampler(samplerInfo);
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
