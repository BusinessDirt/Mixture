#pragma once
#if defined(OPAL_PLATFORM_DARWIN)

/**
 * @file Context.hpp
 * @brief Metal implementation of the Graphics Context.
 */

#include "Mixture/Render/RHI/RHI.hpp"
#include "Platform/Metal/Definitions.hpp"
#include "Platform/Metal/Device.hpp"
#include "Platform/Metal/Swapchain.hpp"
#include "Platform/Metal/Command/CommandList.hpp"

#include <vector>
#include <optional>
#include <array>
#include <functional>

namespace Mixture::Metal
{
    class Context : public RHI::IGraphicsContext
    {
    public:
        Context(const ApplicationDescription& appDescription, void* windowHandle);
        ~Context();

        RHI::GraphicsAPI GetAPI() const override { return RHI::GraphicsAPI::Metal; }

        /**
         * @brief Gets the Metal Device implementation.
         *
         * @return RHI::IGraphicsDevice& The device.
         */
        RHI::IGraphicsDevice& GetDevice() const override;

        /**
         * @brief Handles window resize events.
         *
         * @param width New width.
         * @param height New height.
         */
        void OnResize(uint32_t width, uint32_t height) override;

        /**
         * @brief Begins the frame and acquires the next image.
         *
         * @return RHI::ITexture* The backbuffer texture.
         */
        RHI::ITexture* BeginFrame() override;

        /**
         * @brief Ends the frame and presents the image.
         */
        void EndFrame() override;

        /**
         * @brief Gets a command buffer for the current frame.
         *
         * @return Scope<RHI::ICommandList> The command buffer.
         */
        Scope<RHI::ICommandList> GetCommandBuffer() override;

        /**
         * @brief Gets the swapchain width.
         *
         * @return uint32_t The width.
         */
        uint32_t GetSwapchainWidth() const override;

        /**
         * @brief Gets the swapchain height.
         *
         * @return uint32_t The height.
         */
        uint32_t GetSwapchainHeight() const override;

        uint32_t GetCurrentFrameIndex() const override { return m_CurrentFrame; }

        /**
         * @brief Gets the singleton context instance.
         *
         * @return Context& Reference to the context.
         */
        static Context& Get();

    private:
        Ref<Device> m_Device;
        Scope<Swapchain> m_Swapchain;
        uint32_t m_CurrentFrame = 0;
    };
}

#endif
