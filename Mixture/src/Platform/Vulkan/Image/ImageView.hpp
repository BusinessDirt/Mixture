#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class Device;

    class ImageView final
    {
    public:

        MX_NON_COPIABLE(ImageView);

        explicit ImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        ~ImageView();

    private:
        const VkImage m_Image;
        const VkFormat m_Format;

        VULKAN_HANDLE(VkImageView, m_ImageView);
    };
}
