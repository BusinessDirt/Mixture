#pragma once

#include "Opal/PlatformDetection.hpp"

#include <memory>
#include <vector>

// DEBUG
#ifdef OPAL_DEBUG
    #if defined(OPAL_PLATFORM_WINDOWS)
        #define OPAL_DEBUGBREAK() __debugbreak()
    #elif defined(OPAL_PLATFORM_LINUX)
        #include <signal.h>
        #define OPAL_DEBUGBREAK() raise(SIGTRAP)
    #elif defined(OPAL_PLATFORM_DARWIN)
        #define OPAL_DEBUGBREAK() __builtin_debugtrap()
    #else
        #error "Platform doesn't support debugbreak yet!"
    #endif
    #define OPAL_ENABLE_ASSERTS
#else
    #define OPAL_DEBUGBREAK()
#endif

#define OPAL_NODISCARD [[nodiscard]] 

#define OPAL_EXPAND_MACRO(x) x
#define OPAL_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << (x))
#define OPAL_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define OPAL_NON_COPIABLE(name) name(const name&) = delete; \
    name& operator=(const name&) = delete; \
    name(name&&) = delete; \
    name& operator=(name&&) = delete

#include "Opal/Assert.hpp"
#include "Opal/Log.hpp"
