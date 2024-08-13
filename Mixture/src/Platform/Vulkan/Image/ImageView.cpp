#include "mxpch.hpp"
#include "ImageView.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{

    ImageView::ImageView(const VkImage image, const VkFormat format, const VkImageAspectFlags aspectFlags)
        : m_Image(image), m_Format(format)
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
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        MX_VK_ASSERT(vkCreateImageView(Context::Get().Device->GetHandle(), &createInfo, nullptr, &m_ImageView),
            "Failed to create VkImageView");
    }

    ImageView::~ImageView()
    {
        if (m_ImageView)
        {
            vkDestroyImageView(Context::Get().Device->GetHandle(), m_ImageView, nullptr);
            m_ImageView = nullptr;
        }
    }

}
