#pragma once

#include "Mixture/Core/Window.h"
#include "Mixture/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Mixture {
	class WindowsWindow : public Window {
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void onUpdate() override;

		unsigned int getWidth() const override { return m_Data.width; }
		unsigned int getHeight() const override { return m_Data.height; }

		// Window attributes
		void setEventCallback(const EventCallbackFn& callback) override { m_Data.eventCallback = callback; }
		void setVSync(bool enabled) override;
		bool isVSync() const override;

		virtual void* getNativeWindow() const { return m_Window; }
	private:
		virtual void init(const WindowProps& props);
		virtual void shutdown();
	private:
		GLFWwindow* m_Window;
		Scope<GraphicsContext> m_Context;

		struct WindowData {
			std::string title;
			unsigned int width = 0, height = 0;
			bool vSync = true;

			EventCallbackFn eventCallback;
		};

		WindowData m_Data;
	};
}

