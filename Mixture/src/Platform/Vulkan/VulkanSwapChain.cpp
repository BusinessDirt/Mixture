#include "mxpch.hpp"
#include "VulkanSwapChain.hpp"

#include "Mixture/Core/Application.hpp"

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanSurface.hpp"

namespace Mixture
{
    VulkanSwapChain::VulkanSwapChain(const VulkanPhysicalDevice& physicalDevice, const VulkanDevice& device, const VulkanSurface& surface)
        : m_Device(device)
    {
        VulkanSwapChainSupportDetails details = physicalDevice.QuerySwapChainSupport();
        
        VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(details.Formats);
        VkPresentModeKHR presentMode = ChoosePresentMode(details.PresentModes);
        VkExtent2D extent = ChooseExtent(details.Capabilities);
        uint32_t imageCount = details.Capabilities.minImageCount + 1;
        
        if (details.Capabilities.maxImageCount > 0 && imageCount > details.Capabilities.maxImageCount)
            imageCount = details.Capabilities.maxImageCount;
        
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface.GetHandle();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        
        VulkanQueueFamilyIndices indices = physicalDevice.FindQueueFamilyIndices();
        uint32_t queueFamilyIndices[] = { indices.Present.value(), indices.Present.value() };

        if (indices.Graphics != indices.Present)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } 
        else 
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }
        
        createInfo.preTransform = details.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        
        MX_VK_ASSERT(vkCreateSwapchainKHR(m_Device.GetHandle(), &createInfo, nullptr, &m_SwapChain),
                     "Failed to create VkSwapchainKHR");
        
        vkGetSwapchainImagesKHR(m_Device.GetHandle(), m_SwapChain, &imageCount, nullptr);
        m_Images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device.GetHandle(), m_SwapChain, &imageCount, m_Images.data());
        
        m_Format = surfaceFormat.format;
        m_Extent = extent;
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        if (m_SwapChain)
        {
            vkDestroySwapchainKHR(m_Device.GetHandle(), m_SwapChain, nullptr);
            m_SwapChain = nullptr;
        }
    }

    VkSurfaceFormatKHR VulkanSwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) 
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapChain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) 
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) return availablePresentMode;
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
        {
            return capabilities.currentExtent;
        } 
        else
        {
            VkExtent2D actualExtent =
            {
                Application::Get().GetWindow().GetWidth(),
                Application::Get().GetWindow().GetHeight()
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
}
