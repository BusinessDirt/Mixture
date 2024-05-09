#pragma once

#include "mxpch.h"

#ifdef MIXTURE_DEBUG
	#if defined(MIXTURE_PLATFORM_WINDOWS)
		#define MX_DEBUGBREAK() __debugbreak()
	#elif defined(MIXTURE_PLATFORM_LINUX)
		#include <signal.h>
		#define MX_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define MX_ENABLE_ASSERTS
#else
	#define MX_DEBUGBREAK()
#endif

#ifdef MX_ENABLE_ASSERTS
	#define MX_ASSERT(x, ...) { if(!(x)) { MX_ERROR("Assertion Failed: {0}", __VA_ARGS__); MX_DEBUGBREAK(); } }
	#define MX_CORE_ASSERT(x, ...) { if(!(x)) { MX_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); MX_DEBUGBREAK(); } }
#else
	#define MX_ASSERT(x, ...)
	#define MX_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
#define MX_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)