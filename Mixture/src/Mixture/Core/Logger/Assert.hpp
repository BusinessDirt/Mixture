#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Core/Logger/Log.hpp"

#include <filesystem>

#ifdef MX_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define MX_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { MX##type##ERROR(msg, __VA_ARGS__); MX_DEBUGBREAK(); } }
	#define MX_INTERNAL_ASSERT_WITH_MSG(type, check, ...) MX_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define MX_INTERNAL_ASSERT_NO_MSG(type, check) MX_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", MX_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define MX_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define MX_INTERNAL_ASSERT_GET_MACRO(...) MX_EXPAND_MACRO( MX_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, MX_INTERNAL_ASSERT_WITH_MSG, MX_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define MX_ASSERT(...) MX_EXPAND_MACRO( MX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define MX_CORE_ASSERT(...) MX_EXPAND_MACRO( MX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )

	// Vulkan creation asserts
	#define MX_VK_ASSERT(result, message) MX_CORE_ASSERT(result == VK_SUCCESS, message)

#else

	#define MX_ASSERT(...)
	#define MX_CORE_ASSERT(...)
	#define MX_VK_ASSERT(result, message) result

#endif
