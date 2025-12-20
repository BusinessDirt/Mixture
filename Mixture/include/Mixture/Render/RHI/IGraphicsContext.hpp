#pragma once

/**
 * @file IGraphicsContext.hpp
 * @brief Interface for the graphics context.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"

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
        OpenGL,
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
         * @param appDescription Description of the application.
         */
        IGraphicsContext(const ApplicationDescription& appDescription) {}
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
        virtual Ref<IGraphicsDevice> GetDevice() const = 0;

        /**
         * @brief Factory method to create a graphics context.
         * 
         * @param appDescription Description of the application.
         * @return Scope<IGraphicsContext> The created context.
         */
        static Scope<IGraphicsContext> Create(const ApplicationDescription& appDescription);
    };
}
