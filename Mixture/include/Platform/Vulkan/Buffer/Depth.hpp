#pragma once

#include "Platform/Vulkan/Base.hpp"

namespace Mixture::Vulkan
{
    class DeviceMemory;
    class Image;
    class ImageView;

    class DepthBuffer final
    {
    public:
        OPAL_NON_COPIABLE(DepthBuffer);

        explicit DepthBuffer(VkExtent2D extent);
        ~DepthBuffer() = default;

        OPAL_NODISCARD VkFormat GetFormat() const { return m_Format; }
        OPAL_NODISCARD const ImageView& GetImageView() const { return *m_ImageView; }

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