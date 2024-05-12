#pragma once

#include "mxpch.h"
#include "Mixture/Core/Base.h"
#include "Mixture/Events/Event.h"

namespace Mixture {
	struct WindowProps {
		std::string title;
		uint32_t width;
		uint32_t height;

		WindowProps(const std::string& title = "Mixture Engine", uint32_t width = 1280, uint32_t height = 720)
			: title(title), width(width), height(height) {}
	};

	// Interface representing a desktop system based Window
	class Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void onUpdate() = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		// Window attributes
		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;

		static Scope<Window> create(const WindowProps& props = WindowProps());
	};
}