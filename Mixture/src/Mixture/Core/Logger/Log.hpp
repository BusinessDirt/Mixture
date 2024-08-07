#pragma once

#include "Mixture/Core/Base.hpp"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/bundled/format.h>
#pragma warning(pop)

namespace Mixture
{
	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

#ifndef MX_DIST

// Core log macros
#define MX_CORE_TRACE(...)     ::Mixture::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define MX_CORE_INFO(...)      ::Mixture::Log::GetCoreLogger()->info(__VA_ARGS__)
#define MX_CORE_WARN(...)      ::Mixture::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define MX_CORE_ERROR(...)     ::Mixture::Log::GetCoreLogger()->error(__VA_ARGS__)
#define MX_CORE_CRITICAL(...)     ::Mixture::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define MX_TRACE(...)   ::Mixture::Log::GetClientLogger()->trace(__VA_ARGS__)
#define MX_INFO(...)    ::Mixture::Log::GetClientLogger()->info(__VA_ARGS__)
#define MX_WARN(...)    ::Mixture::Log::GetClientLogger()->warn(__VA_ARGS__)
#define MX_ERROR(...)   ::Mixture::Log::GetClientLogger()->error(__VA_ARGS__)
#define MX_CRITICAL(...)   ::Mixture::Log::GetClientLogger()->critical(__VA_ARGS__)

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