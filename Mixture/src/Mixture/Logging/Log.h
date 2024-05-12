#pragma once

#include "Mixture/Core/Base.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Mixture {
	class Log {
	public:
		static void Init();
		
		static Ref<spdlog::logger>& getCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& getClientLogger() { return s_ClientLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

#ifndef MIXTURE_DIST

// Core log macros
#define MX_CORE_TRACE(...)     ::Mixture::Log::getCoreLogger()->trace(__VA_ARGS__)
#define MX_CORE_INFO(...)      ::Mixture::Log::getCoreLogger()->info(__VA_ARGS__)
#define MX_CORE_WARN(...)      ::Mixture::Log::getCoreLogger()->warn(__VA_ARGS__)
#define MX_CORE_ERROR(...)     ::Mixture::Log::getCoreLogger()->error(__VA_ARGS__)
#define MX_CORE_CRITICAL(...)     ::Mixture::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define MX_TRACE(...)   ::Mixture::Log::getClientLogger()->trace(__VA_ARGS__)
#define MX_INFO(...)    ::Mixture::Log::getClientLogger()->info(__VA_ARGS__)
#define MX_WARN(...)    ::Mixture::Log::getClientLogger()->warn(__VA_ARGS__)
#define MX_ERROR(...)   ::Mixture::Log::getClientLogger()->error(__VA_ARGS__)
#define MX_CRITICAL(...)   ::Mixture::Log::getClientLogger()->critical(__VA_ARGS__)

#else // strip logging in dist build

// Core log macros
#define MX_CORE_TRACE(...)
#define MX_CORE_INFO(...)
#define MX_CORE_WARN(...)
#define MX_CORE_ERROR(...)
#define MX_CORE_FATAL(...)

// Client log macros
#define MX_TRACE(...)
#define MX_INFO(...)
#define MX_WARN(...)
#define MX_ERROR(...)
#define MX_FATAL(...)

#endif