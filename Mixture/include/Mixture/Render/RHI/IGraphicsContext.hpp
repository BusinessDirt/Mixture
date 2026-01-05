#pragma once

/**
 * @file IGraphicsContext.hpp
 * @brief Interface for the graphics context.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"
#include "Mixture/Render/RHI/ICommandList.hpp"

namespace Mixture
{
    struct ApplicationDescription;
}

namespace Mixture::RHI
{
    /**
     * @brief Enum representing supported graphics APIs.
     */
    enum class GraphicsAPI : uint8_t
    {
        None = 0,
        Vulkan,
        D3D12,
        Metal
    };

    /**
     * @brief Interface for the graphics context.
     *
     * Handles initialization of the graphics API and creation of the device.
     */
    class IGraphicsContext
    {
    public:
        /**
         * @brief Constructor.
         *
         */
        IGraphicsContext() = default;
        virtual ~IGraphicsContext() = default;

        /**
         * @brief Gets the current graphics API.
         *
         * @return GraphicsAPI The active API.
         */
        virtual GraphicsAPI GetAPI() const = 0;

        /**
         * @brief Gets the graphics device.
         *
         * @return Ref<IGraphicsDevice> Reference to the device.
         */
        virtual IGraphicsDevice& GetDevice() const = 0;

        /**
         * @brief Called when the window is resized.
         *
         * @param width The new width.
         * @param height The new height.
         */
        virtual void OnResize(uint32_t width, uint32_t height) = 0;

        /**
         * @brief Begins a new frame and acquires the next swapchain image.
         *
         * @return RHI::ITexture* Pointer to the current backbuffer texture.
         */
        virtual RHI::ITexture* BeginFrame() = 0;

        /**
         * @brief Ends the current frame and presents the swapchain image.
         */
        virtual void EndFrame() = 0;

        /**
         * @brief Gets a command buffer for the current frame.
         *
         * @return Scope<RHI::ICommandList> The command buffer.
         */
        virtual Scope<RHI::ICommandList> GetCommandBuffer() = 0;

        /**
         * @brief Gets the current width of the swapchain.
         *
         * @return uint32_t The width.
         */
        virtual uint32_t GetSwapchainWidth() const = 0;

        /**
         * @brief Gets the current height of the swapchain.
         *
         * @return uint32_t The height.
         */
        virtual uint32_t GetSwapchainHeight() const = 0;

        virtual void BeginImGuiFrame() = 0;
        virtual void EndImGuiFrame() = 0;
        virtual void RenderImGui(RHI::ICommandList* cmd) = 0;

        /**
         * @brief Factory method to create a graphics context.
         *
         * @param appDescription Description of the application.
         * @return Scope<IGraphicsContext> The created context.
         */
        static Scope<IGraphicsContext> Create(const ApplicationDescription& appDescription, void* windowHandle);
    };
}
