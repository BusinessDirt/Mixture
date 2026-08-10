#include "mxpch.hpp"
#include "Platform/Vulkan/Resources/Texture.hpp"
#include "Platform/Vulkan/Resources/AllocationPolicy.hpp"

#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Queue.hpp"
#include "Platform/Vulkan/SingleTimeCommand.hpp"
#include "Platform/Vulkan/ResourcePolicy.hpp"

#include <stdexcept>

namespace Mixture::Vulkan
{
    Texture::Texture(Ref<Device> device, const RHI::TextureDesc& spec, std::span<const std::byte> data)
        : m_Device(std::move(device)), m_Width(spec.Width), m_Height(spec.Height), m_Format(spec.PixelFormat),
          m_Usage(spec.Usage), m_DebugName(spec.DebugName), m_OwnsImage(true)
    {
        if (!m_Device) throw std::invalid_argument("Texture requires an owning device");
        if (!data.empty()) m_Usage |= RHI::TextureUsage::TransferDestination;
        Invalidate();

        if (!data.empty())
        {
            const VkDeviceSize imageSize = data.size();

            auto allocator = m_Device->GetAllocator();

            // Staging Buffer
            VkBufferCreateInfo bufferInfo = {};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = imageSize;
            bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            VmaAllocationCreateInfo allocInfo = AllocationPolicy::Upload(true);

            VkBuffer stagingBuffer;
            VmaAllocation stagingAllocation;
            VmaAllocationInfo stagingAllocInfo;

            if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &stagingBuffer, &stagingAllocation, &stagingAllocInfo) != VK_SUCCESS)
            {
                Release();
                throw std::runtime_error("Failed to create Vulkan texture upload staging buffer");
            }

            // Copy data to staging buffer
            if (!stagingAllocInfo.pMappedData)
            {
                vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
                Release();
                throw std::runtime_error("Vulkan texture upload staging buffer was not mapped");
            }
            memcpy(stagingAllocInfo.pMappedData, data.data(), data.size());

            // Upload to Image
            const vk::Image destinationImage = m_Image;
            const vk::ImageAspectFlags aspect = GetImageAspect(m_Format);
            RHI::ResourceState finalState = spec.InitialState;
            if (finalState == RHI::ResourceState::Undefined)
            {
                if (RHI::HasUsage(m_Usage, RHI::TextureUsage::Storage)) finalState = RHI::ResourceState::UnorderedAccess;
                else if (RHI::HasUsage(m_Usage, RHI::TextureUsage::DepthStencilAttachment)) finalState = RHI::ResourceState::DepthStencilWrite;
                else if (RHI::HasUsage(m_Usage, RHI::TextureUsage::ColorAttachment)) finalState = RHI::ResourceState::RenderTarget;
                else finalState = RHI::ResourceState::ShaderResource;
            }
            const ResourceStateMapping finalMapping = MapResourceState(finalState);
            try
            {
                m_UploadCompletion = SingleTimeCommand::Submit(m_Device->GetTransferQueue(),
                    [stagingBuffer, destinationImage, width = m_Width, height = m_Height, aspect, finalMapping](vk::CommandBuffer cmd)
                {
                    vk::ImageMemoryBarrier barrier{};
                    barrier.oldLayout = vk::ImageLayout::eUndefined;
                    barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
                    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.image = destinationImage;
                    barrier.subresourceRange.aspectMask = aspect;
                    barrier.subresourceRange.baseMipLevel = 0;
                    barrier.subresourceRange.levelCount = 1;
                    barrier.subresourceRange.baseArrayLayer = 0;
                    barrier.subresourceRange.layerCount = 1;
                    barrier.srcAccessMask = vk::AccessFlagBits::eNone;
                    barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
                    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
                        vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

                    vk::BufferImageCopy region{};
                    region.imageSubresource.aspectMask = aspect;
                    region.imageSubresource.mipLevel = 0;
                    region.imageSubresource.baseArrayLayer = 0;
                    region.imageSubresource.layerCount = 1;
                    region.imageExtent = vk::Extent3D{width, height, 1};
                    cmd.copyBufferToImage(vk::Buffer(stagingBuffer), destinationImage,
                        vk::ImageLayout::eTransferDstOptimal, 1, &region);

                    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
                    barrier.newLayout = finalMapping.Layout;
                    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
                    barrier.dstAccessMask = finalMapping.Access;
                    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, finalMapping.Stages,
                        vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);
                }, [allocator, stagingBuffer, stagingAllocation]()
                {
                    vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
                });
            }
            catch (...)
            {
                vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
                Release();
                throw;
            }
        }
    }

    Texture::Texture(Ref<Device> device, vk::Format format, vk::Image image, vk::ImageView imageView,
        uint32_t width, uint32_t height)
        : m_Device(std::move(device)), m_Format(EnumMapper::MapFormat(format)), m_Image(image)
        , m_ImageView(imageView), m_Width(width), m_Height(height)
        , m_OwnsImage(false)
    {
        if (!m_Device || !m_Image || !m_ImageView || width == 0 || height == 0)
            throw std::invalid_argument("Swapchain texture wrapper requires valid device, image, view, and dimensions");
    }

    Texture::~Texture()
    {
        Release();
    }

    void Texture::Release()
    {
        if (m_UploadCompletion.valid())
        {
            m_UploadCompletion.wait();
            m_UploadCompletion = {};
        }
        if (m_OwnsImage)
        {
            auto device = m_Device->GetHandle();
            auto allocator = m_Device->GetAllocator();

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

        auto& device = *m_Device;
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
        imageInfo.usage = static_cast<VkImageUsageFlags>(MapTextureUsage(m_Usage));

        // Allocation Info (VMA)
        VmaAllocationCreateInfo allocInfo = AllocationPolicy::DeviceLocal();

        // Create Image
        // (Cast to C handles for VMA)
        VkImage rawImage;
        if (vmaCreateImage(allocator, &imageInfo, &allocInfo, &rawImage, &m_Allocation, nullptr) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate Vulkan texture image");

        m_Image = rawImage;

        // Create View
        vk::ImageViewCreateInfo viewInfo;
        viewInfo.image = m_Image;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = vk::Format(imageInfo.format);
        viewInfo.subresourceRange.aspectMask = GetImageAspect(m_Format);
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        try
        {
            m_ImageView = device.GetHandle().createImageView(viewInfo);
        }
        catch (...)
        {
            vmaDestroyImage(allocator, m_Image, m_Allocation);
            m_Image = nullptr;
            m_Allocation = nullptr;
            throw;
        }

        // Create Sampler (Optional, but usually needed for textures)
        vk::SamplerCreateInfo samplerInfo;
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = m_Device->GetPhysicalDevice().GetProperties().limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;

        try
        {
            m_Sampler = device.GetHandle().createSampler(samplerInfo);
        }
        catch (...)
        {
            device.GetHandle().destroyImageView(m_ImageView);
            vmaDestroyImage(allocator, m_Image, m_Allocation);
            m_Image = nullptr;
            m_ImageView = nullptr;
            m_Allocation = nullptr;
            throw;
        }
    }

    vk::DescriptorImageInfo Texture::GetDescriptorInfo() const
    {
        vk::DescriptorImageInfo info;
        info.imageLayout = RHI::HasUsage(m_Usage, RHI::TextureUsage::Storage)
            ? vk::ImageLayout::eGeneral : vk::ImageLayout::eShaderReadOnlyOptimal;
        info.imageView = m_ImageView;
        info.sampler = m_Sampler;
        return info;
    }
}
