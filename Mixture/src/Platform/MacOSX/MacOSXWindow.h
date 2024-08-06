#pragma once

#include "Mixture/Core/Window.h"

#include <GLFW/glfw3.h>

namespace Mixture
{
	class MacOSXWindow : public Window
	{
	public:
		MacOSXWindow(const WindowProps& props);
		virtual ~MacOSXWindow();

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }
		void* GetNativeWindow() const override { return m_Window; }

        void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

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