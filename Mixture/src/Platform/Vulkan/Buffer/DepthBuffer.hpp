#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class DeviceMemory;
    class Image;
    class ImageView;

    class DepthBuffer final
    {
    public:
        MX_NON_COPIABLE(DepthBuffer);

        DepthBuffer(VkExtent2D extent);
        ~DepthBuffer();

        VkFormat GetFormat() const { return m_Format; }
        const ImageView& GetImageView() const { return *m_ImageView; }

        static bool HasStencilComponent(const VkFormat format)
        {
            return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
        }
        
        static VkFormat FindDepthFormat();

    private:
        const VkFormat m_Format;
        Scope<Image> m_Image;
        Scope<DeviceMemory> m_ImageMemory;
        Scope<ImageView> m_ImageView;
    };

}
