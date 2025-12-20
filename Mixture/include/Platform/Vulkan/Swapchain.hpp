#pragma once
#include "Mixture/Core/Base.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"
#include "Platform/Vulkan/Surface.hpp"

#include <vulkan/vulkan.hpp>
#include <vector>
#include <memory>

namespace Mixture::Vulkan
{

    /**
     * @class Swapchain
     * @brief Manages the swapchain images and presentation to the surface.
     *
     * Handles creation, recreation (on resize), acquiring images for rendering,
     * and presenting rendered images to the screen.
     */
    class Swapchain
    {
    public:
        /**
         * @brief Constructs a Swapchain.
         * @param physicalDevice Reference to the physical device.
         * @param device Reference to the logical device.
         * @param surface Reference to the window surface.
         * @param width Initial width of the swapchain.
         * @param height Initial height of the swapchain.
         */
        Swapchain(Ref<PhysicalDevice> physicalDevice, Ref<Device> device, Ref<Surface> surface, uint32_t width, uint32_t height);

        /**
         * @brief Destroys the Swapchain and cleans up associated resources (image views, etc.).
         */
        ~Swapchain();

        /**
         * @brief Recreates the swapchain, typically called when the window is resized.
         * @param width New width of the swapchain.
         * @param height New height of the swapchain.
         */
        void Recreate(uint32_t width, uint32_t height);

        /**
         * @brief Acquires the next available image index from the swapchain.
         * @param[out] outImageIndex The index of the acquired image.
         * @param semaphore A semaphore to signal when the image is available.
         * @return true if successful, false if the swapchain is out of date (needs resize).
         */
        bool AcquireNextImage(uint32_t& outImageIndex, vk::Semaphore semaphore);

        /**
         * @brief Submits the image to the presentation queue.
         * @param imageIndex The index of the image to present.
         * @param waitSemaphore A semaphore to wait on before presenting (rendering finished).
         * @return true if successful, false if the swapchain is out of date or suboptimal.
         */
        bool Present(uint32_t imageIndex, vk::Semaphore waitSemaphore);

        /**
         * @brief Gets the underlying Vulkan Swapchain handle.
         * @return The vk::SwapchainKHR handle.
         */
        vk::SwapchainKHR GetHandle() const { return m_Swapchain; }

        /**
         * @brief Gets the format of the swapchain images.
         * @return The vk::Format of the images.
         */
        vk::Format GetImageFormat() const { return m_ImageFormat; }

        /**
         * @brief Gets the extent (resolution) of the swapchain images.
         * @return The vk::Extent2D of the images.
         */
        vk::Extent2D GetExtent() const { return m_Extent; }

        /**
         * @brief Gets the image views associated with the swapchain images.
         * @return A vector of vk::ImageView handles.
         */
        const Vector<vk::ImageView>& GetImageViews() const { return m_ImageViews; }

        /**
         * @brief Gets the raw swapchain images.
         * @note These images are owned by the swapchain/driver, do not manually destroy them.
         * @return A vector of vk::Image handles.
         */
        const Vector<vk::Image>& GetImages() const { return m_Images; }

        /**
         * @brief Gets the number of images in the swapchain.
         * @return The number of images.
         */
        uint32_t GetImageCount() const { return static_cast<uint32_t>(m_Images.size()); }

        Ref<RHI::ITexture> GetTexture(uint32_t index) const { return m_SwapchainTextures[index]; }

    private:
        void CreateSwapchain(uint32_t width, uint32_t height);
        void CreateImageViews();

        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const Vector<vk::SurfaceFormatKHR>& availableFormats);
        vk::PresentModeKHR ChooseSwapPresentMode(const Vector<vk::PresentModeKHR>& availablePresentModes);
        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

    private:
        Ref<PhysicalDevice> m_PhysicalDevice;
        Ref<Device> m_Device;
        Ref<Surface> m_Surface;

        vk::PresentModeKHR m_PresentMode;
        vk::SwapchainKHR m_Swapchain;
        vk::Format m_ImageFormat;
        vk::ColorSpaceKHR m_ColorSpace;
        vk::Extent2D m_Extent;

        Vector<Ref<RHI::ITexture>> m_SwapchainTextures;
        Vector<vk::Image> m_Images;
        Vector<vk::ImageView> m_ImageViews;

        vk::SwapchainKHR m_OldSwapchain = nullptr;
    };
}
