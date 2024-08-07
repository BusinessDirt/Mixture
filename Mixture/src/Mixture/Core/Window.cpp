#include "mxpch.hpp"
#include "Window.hpp"

#ifdef MX_PLATFORM_MACOSX
    #include "Platform/MacOSX/MacOSXWindow.hpp"
#endif

namespace Mixture
{
    Scope<Window> Window::Create(const WindowProps& props)
    {
#ifdef MX_PLATFORM_MACOSX
		return CreateScope<MacOSXWindow>(props);
#else
		MX_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
    }
}
