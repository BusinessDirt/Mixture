#pragma once

#include <sstream>

#include "Mixture/Core/Base.hpp"
#include "Mixture/Events/Event.hpp"

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Mixture
{
    struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Mixture Engine", uint32_t width = 1280, uint32_t height = 720)
			: Title(title), Width(width), Height(height)
		{}
	};

	class Window
	{
	public:
        using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowProps& props);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		void OnUpdate();

		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }
        float GetAspectRatio() const { return static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()); }
		GLFWwindow* GetNativeWindow() const { return m_WindowHandle; }

        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
        
        void CreateSurface(VkInstance instance, VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const;

	private:
		struct WindowData
		{
			std::string Title;
			unsigned int Width = 0, Height = 0;
			bool VSync = true;
			bool Minimized = false;
			EventCallbackFn EventCallback;
		};

	private:
		WindowData m_Data;
		GLFWwindow* m_WindowHandle;
	};
}
