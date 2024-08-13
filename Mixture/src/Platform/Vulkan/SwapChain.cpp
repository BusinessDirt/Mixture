#include "mxpch.hpp"
#include "SwapChain.hpp"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Sync/Semaphore.hpp"
#include "Platform/Vulkan/Sync/Fence.hpp"

#include "Mixture/Core/Application.hpp"

namespace Mixture::Vulkan
{
    SwapChain::SwapChain()
    {
        Init();
    }

    SwapChain::SwapChain(Ref<SwapChain> previous)
        : m_OldSwapChain(previous)
    {
        Init();

        m_OldSwapChain = nullptr;
    }

    void SwapChain::Init()
    {
        SwapChainSupportDetails swapChainSupport = Context::Get().PhysicalDevice->QuerySwapChainSupport();

        VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(swapChainSupport.Formats);
        VkPresentModeKHR presentMode = ChoosePresentMode(swapChainSupport.PresentModes);
        VkExtent2D extent = ChooseExtent(swapChainSupport.Capabilities);

        uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
        if (swapChainSupport.Capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.Capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.Capabilities.maxImageCount;
        }

        // Swapchain
        {
            VkSwapchainCreateInfoKHR createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = Context::Get().Surface->GetHandle();

            createInfo.minImageCount = imageCount;
            createInfo.imageFormat = surfaceFormat.format;
            createInfo.imageColorSpace = surfaceFormat.colorSpace;
            createInfo.imageExtent = extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            QueueFamilyIndices indices = Context::Get().PhysicalDevice->FindQueueFamilyIndices();
            uint32_t queueFamilyIndices[] = { indices.Graphics.value(), indices.Present.value() };

            if (indices.Graphics.value() != indices.Present.value())
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices = queueFamilyIndices;
            }
            else
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0;      // Optional
                createInfo.pQueueFamilyIndices = nullptr;  // Optional
            }

            createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

            createInfo.presentMode = presentMode;
            createInfo.clipped = VK_TRUE;

            createInfo.oldSwapchain = m_OldSwapChain ? m_OldSwapChain->m_SwapChain : VK_NULL_HANDLE;

            MX_VK_ASSERT(vkCreateSwapchainKHR(Context::Get().Device->GetHandle(), &createInfo, nullptr, &m_SwapChain),
                "Failed to create VkSwapChainKHR");
        }

        // we only specified a minimum number of images in the swap chain, so the implementation is
        // allowed to create a swap chain with more. That's why we'll first query the final number of
        // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
        // retrieve the handles.
        vkGetSwapchainImagesKHR(Context::Get().Device->GetHandle(), m_SwapChain, &imageCount, nullptr);
        std::vector<VkImage> images(imageCount);
        vkGetSwapchainImagesKHR(Context::Get().Device->GetHandle(), m_SwapChain, &imageCount, images.data());
        
        m_Extent = extent;
        m_RenderPass = CreateScope<RenderPass>(surfaceFormat.format);
        
        // Depth and frame buffers
        m_DepthBuffers.resize(imageCount);
        m_FrameBuffers.resize(imageCount);
        for (size_t i = 0; i < imageCount; i++)
        {
            // Depth Buffers
            m_DepthBuffers[i] = CreateScope<DepthBuffer>(m_Extent);
            
            // Frame Buffers
            m_FrameBuffers[i] =  CreateScope<FrameBuffer>(m_DepthBuffers[i]->GetImageView().GetHandle(), images[i],
                m_Extent, surfaceFormat.format, m_RenderPass->GetHandle());
        }
        
        // Sync objects
        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.reserve(MAX_FRAMES_IN_FLIGHT);
        m_ImagesInFlight.resize(GetImageCount(), nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) m_InFlightFences.emplace_back(true);
    }

    SwapChain::~SwapChain() 
    {
        for (auto& framebuffer : m_FrameBuffers) framebuffer = nullptr;
        for (auto& depthbuffer : m_DepthBuffers) depthbuffer = nullptr;
        
        if (m_SwapChain)
        {
            vkDestroySwapchainKHR(Context::Get().Device->GetHandle(), m_SwapChain, nullptr);
            m_SwapChain = nullptr;
        }

        m_RenderPass = nullptr;

        // cleanup synchronization objects
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            m_ImageAvailableSemaphores[i].~Semaphore();
            m_RenderFinishedSemaphores[i].~Semaphore();
            m_InFlightFences[i].~Fence();
            m_ImagesInFlight[i] = nullptr;
        }
    }

    VkResult SwapChain::AcquireNextImage(uint32_t* imageIndex) 
    {
        m_InFlightFences[m_CurrentFrame].Wait(std::numeric_limits<uint64_t>::max());

        return vkAcquireNextImageKHR(Context::Get().Device->GetHandle(), m_SwapChain, std::numeric_limits<uint64_t>::max(),
            m_ImageAvailableSemaphores[m_CurrentFrame].GetHandle(), VK_NULL_HANDLE, imageIndex);
    }

    VkResult SwapChain::SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex) 
    {
        if (m_ImagesInFlight[*imageIndex] != nullptr)
        {
            m_ImagesInFlight[*imageIndex]->Wait(std::numeric_limits<uint64_t>::max());
        }
        m_ImagesInFlight[*imageIndex] = &m_InFlightFences[m_CurrentFrame];

        VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame].GetHandle() };
        VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame].GetHandle() };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        m_InFlightFences[m_CurrentFrame].Reset();
        MX_VK_ASSERT(vkQueueSubmit(Context::Get().Device->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame].GetHandle()),
            "Failed to submit draw command buffer");

        VkSwapchainKHR swapChains[] = { m_SwapChain };
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = imageIndex;

        VkResult result = vkQueuePresentKHR(Context::Get().Device->GetPresentQueue(), &presentInfo);

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        return result;
    }

    VkSurfaceFormatKHR SwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) 
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR SwapChain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) 
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
            {
                MX_CORE_INFO("Present mode: Mailbox");
                return availablePresentMode;
            }
        }

        MX_CORE_INFO("Present mode: V-Sync");
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
        {
            return capabilities.currentExtent;
        }
        else 
        {
            VkExtent2D actualExtent = { Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() };
            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }
}
