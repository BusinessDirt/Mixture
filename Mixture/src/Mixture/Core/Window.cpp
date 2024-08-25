#include "mxpch.hpp"
#include "Window.hpp"

#ifdef MX_PLATFORM_MACOSX
    #include "Platform/MacOSX/MacOSXWindow.hpp"
#endif

#ifdef MX_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsWindow.hpp"
#endif

namespace Mixture
{
    Scope<Window> Window::Create(const WindowProps& props)
    {
#if defined(MX_PLATFORM_MACOSX)
		return CreateScope<MacOSXWindow>(props);
#elif defined(MX_PLATFORM_WINDOWS)
        return CreateScope<WindowsWindow>(props);
#else
		MX_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
    }
}
