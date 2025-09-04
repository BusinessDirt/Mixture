#include "mxpch.hpp"
#include "Platform/Vulkan/Image.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Buffer/Buffer.hpp"
#include "Platform/Vulkan/Buffer/Depth.hpp"
#include "Platform/Vulkan/Command/SingleTime.hpp"

namespace Mixture::Vulkan
{
    Image::Image(const VkExtent2D extent, const VkFormat format, const uint32_t mipLevels)
        : Image(extent, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, mipLevels)
    {}

    Image::Image(const VkExtent2D extent, const VkFormat format, const VkImageTiling tiling, const VkImageUsageFlags usage, const uint32_t mipLevels)
        : m_MipLevels(mipLevels), m_Extent(extent), m_Format(format), m_ImageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
    {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = m_Extent.width;
        imageInfo.extent.height = m_Extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = m_MipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = m_Format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = m_ImageLayout;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional

        VK_ASSERT(vkCreateImage(Context::Device->GetHandle(), &imageInfo, nullptr, &m_Image),
                  "Mixture::Vulkan::Image::Image() - Creation Failed!")
    }

    Image::Image(Image&& other) noexcept
        : m_MipLevels(0), m_Extent(other.m_Extent), m_Format(other.m_Format), m_ImageLayout(other.m_ImageLayout),
          m_Image(other.m_Image)
    {
        other.m_Image = nullptr;
    }

    Image::~Image()
    {
        if (m_Image)
        {
            vkDestroyImage(Context::Device->GetHandle(), m_Image, nullptr);
            m_Image = nullptr;
        }
    }

    DeviceMemory Image::AllocateMemory(const VkMemoryPropertyFlags properties) const
    {
        const auto requirements = GetMemoryRequirements();
        DeviceMemory memory(requirements.size, requirements.memoryTypeBits, 0, properties);

        VK_ASSERT(vkBindImageMemory(Context::Device->GetHandle(), m_Image, memory.GetHandle(), 0),
                  "Mixture::Vulkan::Image::AllocateMemory() - Memory binding failed!")

        return memory;
    }

    VkMemoryRequirements Image::GetMemoryRequirements() const
    {
        VkMemoryRequirements requirements;
        vkGetImageMemoryRequirements(Context::Device->GetHandle(), m_Image, &requirements);
        return requirements;
    }

    void Image::TransitionImageLayout(const VkCommandBuffer commandBuffer, const VkImage image, const VkImageLayout oldLayout,
                                      const VkImageLayout newLayout, const VkFormat imageFormat, const uint32_t mipLevels)
    {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (DepthBuffer::HasStencilComponent(imageFormat))
            {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags sourceStage = 0;
        VkPipelineStageFlags destinationStage = 0;
        
        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED)
        {
            if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            else if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            }
            else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            }
            else
            {
                OPAL_CORE_ERROR("Mixture::Vulkan::Image::TransitionImageLayout() - Unsupported layout transition! (from VK_IMAGE_LAYOUT_UNDEFINED)");
            }

        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            else
            {
                OPAL_CORE_ERROR("Mixture::Vulkan::Image::TransitionImageLayout() - Unsupported layout transition! (from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)");
            }
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
            {
                barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                barrier.dstAccessMask = 0;

                sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            }
            else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
            {
                barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            else
            {
                OPAL_CORE_ERROR("Mixture::Vulkan::Image::TransitionImageLayout() - Unsupported layout transition! (from VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)");
            }
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            {
                barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            else
            {
                OPAL_CORE_ERROR("Mixture::Vulkan::Image::TransitionImageLayout() - Unsupported layout transition! (from VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)");
            }
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            else
            {
                OPAL_CORE_ERROR("Mixture::Vulkan::Image::TransitionImageLayout() - Unsupported layout transition! (from VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)");
            }
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
            {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = 0;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            }
            else
            {
                OPAL_CORE_ERROR("Mixture::Vulkan::Image::TransitionImageLayout() - Unsupported layout transition! (from VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)");
            }
        }
        else
        {
            OPAL_CORE_ERROR("Mixture::Vulkan::Image::TransitionImageLayout() - Unsupported layout transition!");
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    void Image::TransitionImageLayout(const VkImageLayout newLayout, const VkCommandBuffer commandBuffer)
    {
        if (commandBuffer == VK_NULL_HANDLE)
        {
            SingleTimeCommand::Submit([&](const VkCommandBuffer cmd)
                {
                    TransitionImageLayout(cmd, m_Image, m_ImageLayout, newLayout, m_Format, m_MipLevels);
                });
        }
        else
        {
            TransitionImageLayout(commandBuffer, m_Image, m_ImageLayout, newLayout, m_Format, m_MipLevels);
        }
        

        m_ImageLayout = newLayout;
    }

    void Image::GenerateMipMaps(const VkFormat imageFormat) const
    {
        // Check if image format supports linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(Context::PhysicalDevice->GetHandle(), imageFormat, &formatProperties);

        OPAL_CORE_ASSERT(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT, 
                         "Mixture::Vulkan::Image::GenerateMipMaps() - Texture image format does not support linear blitting!")

        SingleTimeCommand::Submit([&](const VkCommandBuffer commandBuffer)
            {
                VkImageMemoryBarrier barrier{};
                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.image = m_Image;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount = 1;
                barrier.subresourceRange.levelCount = 1;

                int32_t mipWidth = static_cast<int32_t>(m_Extent.width);
                int32_t mipHeight = static_cast<int32_t>(m_Extent.height);

                for (uint32_t i = 1; i < m_MipLevels; i++)
                {
                    barrier.subresourceRange.baseMipLevel = i - 1;
                    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                        0, nullptr, 0, nullptr, 1, &barrier);

                    VkImageBlit blit{};
                    blit.srcOffsets[0] = {.x = 0, .y = 0, .z = 0 };
                    blit.srcOffsets[1] = {.x = mipWidth, .y = mipHeight, .z = 1 };
                    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    blit.srcSubresource.mipLevel = i - 1;
                    blit.srcSubresource.baseArrayLayer = 0;
                    blit.srcSubresource.layerCount = 1;
                    blit.dstOffsets[0] = {.x = 0, .y = 0, .z = 0 };
                    blit.dstOffsets[1] = {.x = mipWidth > 1 ? mipWidth / 2 : 1, .y = mipHeight > 1 ? mipHeight / 2 : 1,
                        .z = 1 };
                    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    blit.dstSubresource.mipLevel = i;
                    blit.dstSubresource.baseArrayLayer = 0;
                    blit.dstSubresource.layerCount = 1;

                    vkCmdBlitImage(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_Image,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

                    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                        0, nullptr, 0, nullptr, 1, &barrier);

                    if (mipWidth > 1) mipWidth /= 2;
                    if (mipHeight > 1) mipHeight /= 2;
                }

                barrier.subresourceRange.baseMipLevel = m_MipLevels - 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                    0, nullptr, 0, nullptr, 1, &barrier);
            });
    }

    void Image::CopyFrom(const Buffer& buffer) const
    {
        SingleTimeCommand::Submit([&](const VkCommandBuffer commandBuffer)
            {
                VkBufferImageCopy region;
                region.bufferOffset = 0;
                region.bufferRowLength = 0;
                region.bufferImageHeight = 0;
                region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount = 1;
                region.imageOffset = {.x = 0, .y = 0, .z = 0 };
                region.imageExtent = {.width = m_Extent.width, .height = m_Extent.height, .depth = 1 };

                vkCmdCopyBufferToImage(commandBuffer, buffer.GetHandle(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
            });
    }

	ImageView::ImageView(const VkImage image, const VkFormat format, const VkImageAspectFlags aspectFlags, const uint32_t mipLevels)
	{
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = aspectFlags;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = mipLevels;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VK_ASSERT(vkCreateImageView(Context::Device->GetHandle(), &createInfo, nullptr, &m_ImageView),
                  "Mixture::Vulkan::ImageView::ImageView() - Creation failed!")
	}

	ImageView::~ImageView()
	{
        if (m_ImageView)
        {
            vkDestroyImageView(Context::Device->GetHandle(), m_ImageView, nullptr);
            m_ImageView = nullptr;
        }
	}

    ImageSampler::ImageSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 16.0f;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        VK_ASSERT(vkCreateSampler(Context::Device->GetHandle(), &samplerInfo, nullptr, &m_Sampler),
                  "Mixture::Vulkan::ImageSampler::ImageSampler() - Creation failed!")
    }

    ImageSampler::~ImageSampler()
    {
        if (m_Sampler) vkDestroySampler(Context::Device->GetHandle(), m_Sampler, nullptr);
    }
}
