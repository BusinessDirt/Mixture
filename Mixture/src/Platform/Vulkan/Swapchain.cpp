#include "mxpch.hpp"
#include "Platform/Vulkan/Swapchain.hpp"
#include "Swapchain.hpp"

namespace Mixture::Vulkan
{
    Swapchain::Swapchain(Ref<Device> device, vk::SurfaceKHR surface, uint32_t width, uint32_t height)
    {

    }

    Swapchain::~Swapchain()
    {

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

    }

    vk::SurfaceFormatKHR Swapchain::ChooseSwapSurfaceFormat(const Vector<vk::SurfaceFormatKHR>& availableFormats)
    {

    }

    vk::PresentModeKHR Swapchain::ChooseSwapPresentMode(const Vector<vk::PresentModeKHR>& availablePresentModes)
    {

    }

    vk::Extent2D Swapchain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, uint32_t width, uint32_t height)
    {
        return vk::Extent2D();
    }
}
