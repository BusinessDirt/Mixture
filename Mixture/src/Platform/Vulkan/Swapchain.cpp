#include "mxpch.hpp"
#include "Platform/Vulkan/Swapchain.hpp"

#include "Platform/Vulkan/Resources/Texture.hpp"

namespace Mixture::Vulkan
{
    Swapchain::Swapchain(Ref<PhysicalDevice> physicalDevice, Ref<Device> device,
        Ref<Surface> surface, uint32_t width, uint32_t height)
        : m_PhysicalDevice(physicalDevice), m_Device(device), m_Surface(surface)
    {
        CreateSwapchain(width, height);
        CreateImageViews();

        OPAL_INFO("Core/Vulkan", "Swapchain Details:");
        OPAL_INFO("Core/Vulkan", " - Surface Format: {}, {}",  m_ImageFormat, m_ColorSpace);
        OPAL_INFO("Core/Vulkan", " - Present Mode: {}", m_PresentMode);
        OPAL_INFO("Core/Vulkan", " - Extent: {} x {}", m_Extent.width, m_Extent.height);
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
        m_Device->WaitForIdle(); // Don't touch resources while GPU is using them

        // Cleanup old views
        for (auto imageView : m_ImageViews) m_Device->GetHandle().destroyImageView(imageView);
        m_ImageViews.clear();
        m_Images.clear();

        // CACHING: Store current swapchain as "Old" so the driver
        //          can copy internal data to the new one.
        m_OldSwapchain = m_Swapchain;

        // Create new Swapchain and destroy the old one
        CreateSwapchain(width, height);
        if (m_OldSwapchain)
        {
            m_Device->GetHandle().destroySwapchainKHR(m_OldSwapchain);
            m_OldSwapchain = nullptr;
        }

        // Re-create Views for the new images
        CreateImageViews();
    }

    bool Swapchain::AcquireNextImage(uint32_t& outImageIndex, vk::Semaphore semaphore)
    {
        // Time out: UINT64_MAX (Wait forever)
        vk::Result result = m_Device->GetHandle().acquireNextImageKHR(
            m_Swapchain, UINT64_MAX, semaphore,
            nullptr, &outImageIndex);

        if (result == vk::Result::eErrorOutOfDateKHR)
        {
            return false; // Resize required
        }
        else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
        {
            OPAL_CRITICAL("Core/Vulkan", "Failed to acquire swapchain image!");
            return false;
        }

        return true;
    }

    bool Swapchain::Present(uint32_t imageIndex, vk::Semaphore waitSemaphore)
    {
        vk::PresentInfoKHR presentInfo;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &waitSemaphore;

        vk::SwapchainKHR swapChains[] = { m_Swapchain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        vk::Result result;
        try
        {
            result = m_Device->GetGraphicsQueue().presentKHR(presentInfo);
        }
        catch (vk::SystemError& err)
        {
            // catch "Out Of Date" exception thrown by hpp
            if (err.code() == vk::Result::eErrorOutOfDateKHR)
            {
                return false;
            }

            throw;
        }

        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
        {
            return false;
        }

        return true;
    }

    void Swapchain::CreateSwapchain(uint32_t width, uint32_t height)
    {
        auto physicalDevice = m_PhysicalDevice->GetHandle();

        // Query Details
        vk::SurfaceCapabilitiesKHR capabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_Surface->GetHandle());
        Vector<vk::SurfaceFormatKHR> formats = physicalDevice.getSurfaceFormatsKHR(m_Surface->GetHandle());
        Vector<vk::PresentModeKHR> presentModes = physicalDevice.getSurfacePresentModesKHR(m_Surface->GetHandle());

        // Choose Settings
        vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(formats);
        vk::PresentModeKHR presentMode = ChooseSwapPresentMode(presentModes);
        vk::Extent2D extent = ChooseSwapExtent(capabilities, width, height);

        // Image Count (Min + 1 is a good rule of thumb for triple buffering)
        uint32_t imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
            imageCount = capabilities.maxImageCount;

        // Create Info
        vk::SwapchainCreateInfoKHR createInfo;
        createInfo.surface = m_Surface->GetHandle();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1; // Always 1 unless VR (stereoscopic)
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment; // We render directly to it
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.preTransform = capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = m_OldSwapchain;

        try
        {
            m_Swapchain = m_Device->GetHandle().createSwapchainKHR(createInfo);
        }
        catch (vk::SystemError& err)
        {
            OPAL_CRITICAL("Core/Vulkan", "Failed to create Swapchain!");
            exit(-1);
        }

        // Store selected properties
        m_PresentMode = presentMode;
        m_ImageFormat = surfaceFormat.format;
        m_ColorSpace = surfaceFormat.colorSpace;
        m_Extent = extent;

        // Retrieve Images
        m_Images = m_Device->GetHandle().getSwapchainImagesKHR(m_Swapchain);
    }

    void Swapchain::CreateImageViews()
    {
        m_ImageViews.resize(m_Images.size());
        m_SwapchainTextures.resize(m_Images.size());

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

            m_SwapchainTextures[i] = CreateRef<Vulkan::Texture>(
                m_Images[i], m_ImageViews[i],
                m_Extent.width, m_Extent.height
            );
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
