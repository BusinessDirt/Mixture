#pragma once

#include "Mixture/Core/Window.hpp"

struct GLFWwindow;

namespace Mixture
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }
		void* GetNativeWindow() const override { return m_Window; }

		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		void CreateSurface(VkInstance instance, VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const override;

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
		GLFWwindow* m_Window;
	};
}
