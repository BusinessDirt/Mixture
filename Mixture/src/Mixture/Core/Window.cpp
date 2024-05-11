#include "mxpch.h"
#include "Mixture/Core/Window.h"

#ifdef MX_PLATFORM_WINDOWS
	#include "Mixture/Platform/Windows/WindowsWindow.h"
#endif

namespace Mixture {
	Scope<Window> Window::create(const WindowProps& props) {
	#ifdef MX_PLATFORM_WINDOWS
		return createScope<WindowsWindow>(props);
	#else
		MX_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
	}
}