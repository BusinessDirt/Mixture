#pragma once
#if defined(OPAL_PLATFORM_DARWIN)

/**
 * @file Swapchain.hpp
 * @brief Metal Swapchain wrapper managing CAMetalLayer and drawables.
 */

#include "Platform/Metal/Definitions.hpp"
#include "Platform/Metal/Resources/Texture.hpp"

namespace Mixture::Metal
{
    class Device;

    /**
     * @brief Manages CAMetalLayer setup and backbuffer textures.
     */
    class Swapchain
    {
    public:
        Swapchain(Device& device, void* windowHandle, uint32_t width, uint32_t height);
        ~Swapchain();

        void Resize(uint32_t width, uint32_t height);
        RHI::ITexture* AcquireNextTexture();
        void Present();

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        MTL::PixelFormat GetPixelFormat() const { return m_PixelFormat; }
        CA::MetalLayer* GetLayer() const { return m_Layer; }

    private:
        Device& m_Device;
        void* m_WindowHandle = nullptr;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        MTL::PixelFormat m_PixelFormat = MTL::PixelFormatBGRA8Unorm;

        CA::MetalLayer* m_Layer = nullptr;
        CA::MetalDrawable* m_CurrentDrawable = nullptr;
        Scope<Texture> m_CurrentTexture;
    };
}

#endif
