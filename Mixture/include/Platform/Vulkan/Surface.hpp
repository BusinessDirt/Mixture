#pragma once

/**
 * @file Surface.hpp
 * @brief Vulkan Surface wrapper.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Instance.hpp"

namespace Mixture::Vulkan
{
    /**
     * @class Surface
     * @brief Represents a Vulkan KHR Surface, serving as the interface between the windowing system and Vulkan.
     */
    class Surface
    {
    public:
        /**
         * @brief Constructs a Surface.
         * @param instance A reference to the Vulkan Instance.
         * @param windowHandle The raw platform-specific window handle (e.g., HWND on Windows, NSWindow* on macOS).
         */
        Surface(Instance& instance, void* windowHandle);

        /**
         * @brief Destroys the Surface.
         */
        ~Surface();

        /**
         * @brief Gets the underlying Vulkan Surface handle.
         * @return The vk::SurfaceKHR handle.
         */
        vk::SurfaceKHR GetHandle() const { return m_Handle; }

    private:
        Instance* m_Instance;
        vk::SurfaceKHR m_Handle;
    };
}
