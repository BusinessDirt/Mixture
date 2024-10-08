#pragma once

#include <sstream>

#include "Mixture/Core/Base.hpp"
#include "Mixture/Events/Event.hpp"

#include <vulkan/vulkan.h>

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

		Window() = default;
		virtual ~Window() = default;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
        float GetAspectRatio() const { return static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()); }
		virtual void* GetNativeWindow() const = 0;

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        
        virtual void CreateSurface(VkInstance instance, VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const = 0;

        static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}
