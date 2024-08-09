#include "mxpch.hpp"
#include "VulkanSwapChain.hpp"

#include "Mixture/Core/Application.hpp"

#include "Platform/Vulkan/VulkanContext.hpp"

namespace Mixture
{
    VulkanSwapChain::VulkanSwapChain()
    {
        VulkanSwapChainSupportDetails details = VulkanContext::Get().PhysicalDevice->QuerySwapChainSupport();
        
        VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(details.Formats);
        VkPresentModeKHR presentMode = ChoosePresentMode(details.PresentModes);
        VkExtent2D extent = ChooseExtent(details.Capabilities);
        uint32_t imageCount = details.Capabilities.minImageCount + 1;
        
        if (details.Capabilities.maxImageCount > 0 && imageCount > details.Capabilities.maxImageCount)
            imageCount = details.Capabilities.maxImageCount;
        
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = VulkanContext::Get().Surface->GetHandle();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        
        VulkanQueueFamilyIndices indices = VulkanContext::Get().PhysicalDevice->FindQueueFamilyIndices();
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
        
        MX_VK_ASSERT(vkCreateSwapchainKHR(VulkanContext::Get().Device->GetHandle(), &createInfo, nullptr, &m_SwapChain),
                     "Failed to create VkSwapchainKHR");
        
        vkGetSwapchainImagesKHR(VulkanContext::Get().Device->GetHandle(), m_SwapChain, &imageCount, nullptr);
        m_Images.resize(imageCount);
        vkGetSwapchainImagesKHR(VulkanContext::Get().Device->GetHandle(), m_SwapChain, &imageCount, m_Images.data());
        
        m_Format = surfaceFormat.format;
        m_Extent = extent;
        
        m_ImageViews.resize(imageCount);
        for (size_t i = 0; i < m_ImageViews.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_Images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_Format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            
            MX_VK_ASSERT(vkCreateImageView(VulkanContext::Get().Device->GetHandle(), &createInfo, nullptr, &m_ImageViews[i]),
                         "Failed to create VkImageView");
        }
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        for (VkImageView& imageView : m_ImageViews)
        {
            if (imageView)
            {
                vkDestroyImageView(VulkanContext::Get().Device->GetHandle(), imageView, nullptr);
                imageView = nullptr;
            }
        }
        
        if (m_SwapChain)
        {
            vkDestroySwapchainKHR(VulkanContext::Get().Device->GetHandle(), m_SwapChain, nullptr);
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
