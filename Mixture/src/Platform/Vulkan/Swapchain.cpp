#include "mxpch.hpp"
#include "Platform/Vulkan/Swapchain.hpp"
#include "Swapchain.hpp"

namespace Mixture::Vulkan
{
    Swapchain::Swapchain(Ref<Device> device, vk::SurfaceKHR surface, uint32_t width, uint32_t height)
        : m_Device(device), m_Surface(surface)
    {
        CreateSwapchain(width, height);
        CreateImageViews();
    }

    Swapchain::~Swapchain()
    {
        for (auto imageView : m_ImageViews)
        {
            m_Device->GetHandle().destroyImageView(imageView);
        }

        m_Device->GetHandle().destroySwapchainKHR(m_Swapchain);
    }

    void Swapchain::Recreate(uint32_t width, uint32_t height)
    {

    }

    bool Swapchain::AcquireNextImage(uint32_t& outImageIndex, vk::Semaphore semaphore)
    {
        return false;
    }

    bool Swapchain::Present(uint32_t imageIndex, vk::Semaphore waitSemaphore)
    {
        return false;
    }

    void Swapchain::CreateSwapchain(uint32_t width, uint32_t height)
    {

    }

    void Swapchain::CreateImageViews()
    {
        m_ImageViews.resize(m_Images.size());

        for (size_t i = 0; i < m_Images.size(); i++)
        {
            vk::ImageViewCreateInfo createInfo;
            createInfo.image = m_Images[i];
            createInfo.viewType = vk::ImageViewType::e2D;
            createInfo.format = m_ImageFormat;

            // Default Mapping (Identity)
            createInfo.components.r = vk::ComponentSwizzle::eIdentity;
            createInfo.components.g = vk::ComponentSwizzle::eIdentity;
            createInfo.components.b = vk::ComponentSwizzle::eIdentity;
            createInfo.components.a = vk::ComponentSwizzle::eIdentity;

            // Subresource Range (Which part of image?)
            createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            m_ImageViews[i] = m_Device->GetHandle().createImageView(createInfo);
        }
    }

    vk::SurfaceFormatKHR Swapchain::ChooseSwapSurfaceFormat(const Vector<vk::SurfaceFormatKHR>& availableFormats)
    {
        // Prefer SRGB (Standard colors) with BGRA or RGBA
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
                availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return availableFormat;
            }
        }

        // Fallback: Just return the first one
        return availableFormats[0];
    }

    vk::PresentModeKHR Swapchain::ChooseSwapPresentMode(const Vector<vk::PresentModeKHR>& availablePresentModes)
    {
        // Prefer Mailbox (Triple Buffering, Low Latency, No Tearing)
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox)
            {
                return availablePresentMode;
            }
        }

        // Fallback: FIFO (VSync) - Guaranteed to be available by spec
        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D Swapchain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities,
        uint32_t width, uint32_t height)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;

        // Match the window size, clamped to hardware limits
        vk::Extent2D actualExtent = { width, height };

        actualExtent.width = std::clamp(actualExtent.width,
            capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
            capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
