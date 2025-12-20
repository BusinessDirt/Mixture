#pragma once
#include "Mixture/Core/Base.hpp"
#include "Platform/Vulkan/Device.hpp"

#include <vulkan/vulkan.hpp>
#include <vector>
#include <memory>

namespace Mixture::Vulkan
{

    class Swapchain
    {
    public:
        // We need the Device (to allocate) and the Surface (to connect to window)
        Swapchain(Ref<Device> device, vk::SurfaceKHR surface, uint32_t width, uint32_t height);
        ~Swapchain();

        // ---------------------------------------------------------------------
        // Core Actions
        // ---------------------------------------------------------------------
        // Called when window resizes
        void Recreate(uint32_t width, uint32_t height);

        // Ask swapchain for the next available image index
        // Returns true if successful, false if swapchain is out of date (resize needed)
        bool AcquireNextImage(uint32_t& outImageIndex, vk::Semaphore semaphore);

        // Submit the image to the presentation queue
        // Returns true if successful, false if resize needed
        bool Present(uint32_t imageIndex, vk::Semaphore waitSemaphore);

        // ---------------------------------------------------------------------
        // Getters
        // ---------------------------------------------------------------------
        vk::SwapchainKHR GetHandle() const { return m_Swapchain; }
        vk::Format GetImageFormat() const { return m_ImageFormat; }
        vk::Extent2D GetExtent() const { return m_Extent; }

        // Used to create Framebuffers/RenderTargets later
        const Vector<vk::ImageView>& GetImageViews() const { return m_ImageViews; }

        // The raw images (owned by the swapchain, don't destroy them!)
        const Vector<vk::Image>& GetImages() const { return m_Images; }
        uint32_t GetImageCount() const { return static_cast<uint32_t>(m_Images.size()); }

    private:
        void CreateSwapchain(uint32_t width, uint32_t height);
        void CreateImageViews();

        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const Vector<vk::SurfaceFormatKHR>& availableFormats);
        vk::PresentModeKHR ChooseSwapPresentMode(const Vector<vk::PresentModeKHR>& availablePresentModes);
        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

    private:
        Ref<Device> m_Device;
        vk::SurfaceKHR m_Surface;

        vk::SwapchainKHR m_Swapchain;
        vk::Format m_ImageFormat;
        vk::Extent2D m_Extent;

        Vector<vk::Image> m_Images;
        Vector<vk::ImageView> m_ImageViews;

        vk::SwapchainKHR m_OldSwapchain = nullptr;
    };
}
