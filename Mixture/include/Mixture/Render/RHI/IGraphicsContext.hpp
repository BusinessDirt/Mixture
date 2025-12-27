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

        virtual void OnResize(uint32_t width, uint32_t height) = 0;
        virtual RHI::ITexture* BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual Scope<RHI::ICommandList> GetCommandBuffer() = 0;

        virtual uint32_t GetSwapchainWidth() const = 0;
        virtual uint32_t GetSwapchainHeight() const = 0;

        /**
         * @brief Factory method to create a graphics context.
         *
         * @param appDescription Description of the application.
         * @return Scope<IGraphicsContext> The created context.
         */
        static Scope<IGraphicsContext> Create(const ApplicationDescription& appDescription, void* windowHandle);
    };
}
