#if defined(OPAL_PLATFORM_DARWIN)
#include "mxpch.hpp"
#include "Platform/Metal/Swapchain.hpp"
#include "Platform/Metal/Device.hpp"

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

namespace Mixture::Metal
{
    Swapchain::Swapchain(Device& device, void* windowHandle, uint32_t width, uint32_t height)
        : m_Device(device), m_WindowHandle(windowHandle), m_Width(width), m_Height(height)
    {
        if (!windowHandle) return;

        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(windowHandle);
        NSWindow* metalWindow = glfwGetCocoaWindow(glfwWindow);

        CAMetalLayer* metalLayer = [CAMetalLayer layer];
        metalLayer.device = (__bridge id<MTLDevice>)device.GetHandle();
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        metalLayer.drawableSize = CGSizeMake(width, height);

        metalWindow.contentView.layer = metalLayer;
        metalWindow.contentView.wantsLayer = YES;

        m_Layer = (__bridge CA::MetalLayer*)metalLayer;
    }

    Swapchain::~Swapchain()
    {
        m_CurrentTexture.reset();
        m_CurrentDrawable = nullptr;
        m_Layer = nullptr;
    }

    void Swapchain::Resize(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        if (m_Layer)
        {
            CAMetalLayer* metalLayer = (__bridge CAMetalLayer*)m_Layer;
            metalLayer.drawableSize = CGSizeMake(width, height);
        }
    }

    RHI::ITexture* Swapchain::AcquireNextTexture()
    {
        if (!m_Layer) return nullptr;

        m_CurrentDrawable = m_Layer->nextDrawable();
        if (!m_CurrentDrawable) return nullptr;

        MTL::Texture* metalTexture = m_CurrentDrawable->texture();
        m_CurrentTexture = CreateScope<Texture>(
            m_Device.shared_from_this(),
            RHI::Format::B8G8R8A8_UNORM,
            metalTexture,
            m_Width,
            m_Height
        );
        return m_CurrentTexture.get();
    }

    void Swapchain::Present()
    {
        if (m_CurrentDrawable)
        {
            id<CAMetalDrawable> drawable = (__bridge id<CAMetalDrawable>)m_CurrentDrawable;
            [drawable present];
            m_CurrentDrawable = nullptr;
        }
    }
}
#endif
