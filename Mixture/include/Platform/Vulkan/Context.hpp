#pragma once

/**
 * @file Context.hpp
 * @brief Vulkan implementation of the Graphics Context.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/RHI.hpp"

#include <vector>
#include <optional>

namespace Mixture::Vulkan
{
    class Instance;
    class Surface;
    class PhysicalDevice;
    class Device;
    class Swapchain;

    class CommandBuffers;
    class CommandList;
    class CommandPool;

    class Fences;
    class Semaphores;

    class DescriptorLayoutCache;
    class DescriptorAllocators;
    class DescriptorAllocator;

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
        RHI::IGraphicsDevice& GetDevice() const override;

        void OnResize(uint32_t width, uint32_t height) override;

        Ref<RHI::ITexture> BeginFrame() override;
        void EndFrame() override;

        Ref<RHI::ICommandList> GetCommandBuffer() override;

        uint32_t GetSwapchainWidth() override;
        uint32_t GetSwapchainHeight() override;

        /**
         * @brief Gets the Vulkan instance.
         *
         * @return Ref<Instance> Reference to the instance wrapper.
         */
        Instance& GetInstance() const { return *m_Instance; }

        /**
         * @brief Gets the Vulkan Window Surface.
         *
         * @return Ref<Surface> Reference to the window surface wrapper.
         */
        Surface& GetSurface() const { return *m_Surface; }

        /**
         * @brief Gets the Vulkan physical device.
         *
         * @return Ref<PhysicalDevice> Reference to the physical device wrapper.
         */
        PhysicalDevice& GetPhysicalDevice() const { return *m_PhysicalDevice; }

        /**
         * @brief Gets the Vulkan logical device.
         *
         * @return Ref<Device> Reference to the device wrapper.
         */
        Device& GetLogicalDevice() const { return *m_Device; }

        /**
         * @brief Gets the Vulkan swapchain.
         *
         * @return Ref<Swapchain> Reference to the swapchain wrapper.
         */
        Swapchain& GetSwapchain() const { return *m_Swapchain; }

        /**
         * @brief Gets the command pool.
         *
         * @return Ref<CommandPool> the command pool.
         */
        CommandPool& GetCommandPool() const { return *m_CommandPool; }

        uint32_t GetCurrentFrameIndex() const { return m_CurrentFrame; }

        DescriptorAllocator* GetCurrentDescriptorAllocator() const;
        DescriptorLayoutCache* GetDescriptorLayoutCache() const;

        /**
         * @brief Gets the singleton context instance.
         *
         * @return Context& Reference to the context.
         */
        static Context& Get();
    private:
        Scope<Instance> m_Instance;
        Scope<Surface> m_Surface;
        Scope<PhysicalDevice> m_PhysicalDevice;
        Scope<Device> m_Device;
        Scope<Swapchain> m_Swapchain;

        Scope<Semaphores> m_ImageAvailableSemaphores;
        Scope<Semaphores> m_RenderFinishedSemaphores;
        Scope<Fences> m_InFlightFences;

        Scope<CommandPool> m_CommandPool;
        Scope<CommandBuffers> m_CommandBuffers;

        Scope<DescriptorAllocators> m_DescriptorAllocators;
        Scope<DescriptorLayoutCache> m_DescriptorLayoutCache;

        uint32_t m_CurrentFrame = 0;
        uint32_t m_ImageIndex = 0;
        bool m_IsFrameStarted = false;
    };
}
