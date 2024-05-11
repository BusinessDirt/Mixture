#include "mxpch.h"
#include "Mixture/Input/Input.h"

#ifdef MX_PLATFORM_WINDOWS
	#include "Mixture/Platform/Windows/WindowsInput.h"
#endif

namespace Mixture {
	Scope<Input> Input::s_Instance = Input::create();

	Scope<Input> Input::create() {
	#ifdef MX_PLATFORM_WINDOWS
		return createScope<WindowsInput>();
	#else
		MX_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
	}
}