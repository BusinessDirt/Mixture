#pragma once

#include "Platform/Vulkan/Base.hpp"
#include "Platform/Vulkan/DeviceMemory.hpp"

namespace Mixture::Vulkan
{
    class Buffer;

    class Image final
    {
    public:
        Image(const Image&) = delete;
        Image& operator = (const Image&) = delete;
        Image& operator = (Image&&) = delete;

        Image(VkExtent2D extent, VkFormat format, uint32_t mipLevels = 1);
        Image(VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, uint32_t mipLevels = 1);
        Image(Image&& other) noexcept;
        ~Image();

        OPAL_NODISCARD VkExtent2D GetExtent() const { return m_Extent; }
        OPAL_NODISCARD VkFormat GetFormat() const { return m_Format; }
        OPAL_NODISCARD uint32_t GetMipLevels() const { return m_MipLevels; }
        OPAL_NODISCARD VkImageLayout GetLayout() const { return m_ImageLayout; }

        OPAL_NODISCARD DeviceMemory AllocateMemory(VkMemoryPropertyFlags properties) const;
        OPAL_NODISCARD VkMemoryRequirements GetMemoryRequirements() const;

        static void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout,
                                          VkImageLayout newLayout, VkFormat imageFormat = VK_FORMAT_UNDEFINED, uint32_t mipLevels = 1);
        
        void TransitionImageLayout(VkImageLayout newLayout, VkCommandBuffer commandBuffer = VK_NULL_HANDLE);
        void GenerateMipMaps(VkFormat imageFormat) const;
        void CopyFrom(const Buffer& buffer) const;

    private:
        uint32_t m_MipLevels;
        VkExtent2D m_Extent;
        VkFormat m_Format;
        VkImageLayout m_ImageLayout;

        VULKAN_HANDLE(VkImage, m_Image);
    };

    class ImageView final
    {
    public:
        OPAL_NON_COPIABLE(ImageView);

        explicit ImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels = 1);
        ~ImageView();

    private:
        VULKAN_HANDLE(VkImageView, m_ImageView);
    };

    class ImageSampler final
    {
    public:
        OPAL_NON_COPIABLE(ImageSampler);

        explicit ImageSampler();
        ~ImageSampler();
    private:
        VULKAN_HANDLE(VkSampler, m_Sampler);
    };
}
