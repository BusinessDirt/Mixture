#pragma once

#include "Mixture/Events/Event.hpp"

#include <string>
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Mixture 
{
    /**
     * @brief Properties for creating a window.
     */
    struct WindowProps
    {
        std::string Title;
        int Width;
        int Height;

        explicit WindowProps(std::string title = "Mixture Window", const int width = 800, const int height = 600)
            : Title(std::move(title)), Width(width), Height(height) {}
    };

    /**
     * @brief Abstract representation of a desktop window.
     * 
     * Uses GLFW internally.
     */
    class Window 
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        OPAL_NON_COPIABLE(Window);
        
        /**
         * @brief Creates a new window.
         * 
         * @param props Window properties.
         */
        explicit Window(const WindowProps& props);
        ~Window();

        OPAL_NODISCARD unsigned int GetWidth() const { return m_Data.Width; }
        OPAL_NODISCARD unsigned int GetHeight() const { return m_Data.Height; }
        OPAL_NODISCARD float GetAspectRatio() const { return m_Data.AspectRatio; }
        OPAL_NODISCARD void* GetNativeWindow() const { return m_WindowHandle; }

        /**
         * @brief Updates the window (polls events, swaps buffers).
         */
        void OnUpdate() const;

        /**
         * @brief Retrieves the framebuffer size.
         * 
         * @param width Pointer to store width.
         * @param height Pointer to store height.
         */
        void GetFramebufferSize(int* width, int* height) const;

        /**
         * @brief Creates a Vulkan surface for this window.
         * 
         * @param instance Vulkan instance.
         * @param allocator Allocation callbacks.
         * @param surface Pointer to store the created surface.
         */
        void CreateVulkanSurface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const;
        
        /**
         * @brief Sets the event callback function.
         * 
         * @param callback The function to call when an event occurs.
         */
        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

    private:
        struct WindowData
        {
            std::string Title;
            unsigned int Width = 0, Height = 0;
            float AspectRatio = 0.0f;
            bool VSync = true;
            bool Minimized = false;
            EventCallbackFn EventCallback;
        };

    private:
        GLFWwindow* m_WindowHandle;
        WindowData m_Data;
        std::function<void(Event&)> m_EventCallback;
    };
}

