#pragma once

#include "Window.h"

struct GLFWwindow;

namespace Mixture::Window {
	class GLFWWindow : public Window {
	public:
		GLFWWindow(const Window::Properties& properties);
		virtual ~GLFWWindow();

		bool shouldClose() override;
		void processEvents() override;
		void close() override;
		float getDpiFactor() const override;
		float getContentScaleFactor() const override;

	private:
		GLFWwindow* m_WindowHandle = nullptr;
	};
}