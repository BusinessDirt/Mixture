#pragma once

/**
 * @file Context.hpp
 * @brief Vulkan implementation of the Graphics Context.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/RHI.hpp"

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Instance.hpp"
#include "Platform/Vulkan/Surface.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Swapchain.hpp"

#include <vulkan/vulkan.hpp>
#include <vector>
#include <iostream>
#include <optional>

namespace Mixture::Vulkan
{
    /**
     * @brief Vulkan implementation of the Graphics Context.
     *
     * Manages the Vulkan instance, physical device, and logical device.
     */
    class Context : public RHI::IGraphicsContext
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param appDescription Description of the application.
         */
        Context(const ApplicationDescription& appDescription, void* windowHandle);
        ~Context();

        RHI::GraphicsAPI GetAPI() const override { return RHI::GraphicsAPI::Vulkan; }
        Ref<RHI::IGraphicsDevice> GetDevice() const override { return m_Device; }

        /**
         * @brief Gets the Vulkan instance.
         *
         * @return Ref<Instance> Reference to the instance wrapper.
         */
        Ref<Instance> GetInstance() const { return m_Instance; }

        /**
         * @brief Gets the Vulkan Window Surface.
         *
         * @return Ref<Surface> Reference to the window surface wrapper.
         */
        Ref<Surface> GetSurface() const { return m_Surface; }

        /**
         * @brief Gets the Vulkan physical device.
         *
         * @return Ref<PhysicalDevice> Reference to the physical device wrapper.
         */
        Ref<PhysicalDevice> GetPhysicalDevice() const { return m_PhysicalDevice; }

        /**
         * @brief Gets the Vulkan logical device.
         *
         * @return Ref<Device> Reference to the device wrapper.
         */
        Ref<Device> GetLogicalDevice() const { return m_Device; }

        /**
         * @brief Gets the Vulkan swapchain.
         *
         * @return Ref<Swapchain> Reference to the swapchain wrapper.
         */
        Ref<Swapchain> GetSwapchain() const { return m_Swapchain; }

        /**
         * @brief Gets the singleton context instance.
         *
         * @return Context& Reference to the context.
         */
        static Context& Get();
    private:
        Ref<Instance> m_Instance;
        Ref<Surface> m_Surface;
        Ref<PhysicalDevice> m_PhysicalDevice;
        Ref<Device> m_Device;
        Ref<Swapchain> m_Swapchain;
    };
}
