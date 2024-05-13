#pragma once

#include <memory>
#include "Mixture/Core/PlatformDetection.h"

#ifdef MX_DEBUG
	#if defined(MX_PLATFORM_WINDOWS)
		#define MX_DEBUGBREAK() __debugbreak()
	#elif defined(MX_PLATFORM_LINUX)
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
#define MX_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Mixture {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> createScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> createRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}