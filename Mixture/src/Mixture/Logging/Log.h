#pragma once

#include <spdlog/spdlog.h>

namespace Mixture::Logging {
	class Log {
	public:
		static void Init();
		
		inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#ifndef MIXTURE_DIST

// Core log macros
#define MX_CORE_TRACE(...)     ::Mixture::Logging::Log::getCoreLogger()->trace(__VA_ARGS__)
#define MX_CORE_INFO(...)      ::Mixture::Logging::Log::getCoreLogger()->info(__VA_ARGS__)
#define MX_CORE_WARN(...)      ::Mixture::Logging::Log::getCoreLogger()->warn(__VA_ARGS__)
#define MX_CORE_ERROR(...)     ::Mixture::Logging::Log::getCoreLogger()->error(__VA_ARGS__)
#define MX_CORE_CRITICAL(...)     ::Mixture::Logging::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define MX_TRACE(...)   ::Mixture::Logging::Log::getClientLogger()->trace(__VA_ARGS__)
#define MX_INFO(...)    ::Mixture::Logging::Log::getClientLogger()->info(__VA_ARGS__)
#define MX_WARN(...)    ::Mixture::Logging::Log::getClientLogger()->warn(__VA_ARGS__)
#define MX_ERROR(...)   ::Mixture::Logging::Log::getClientLogger()->error(__VA_ARGS__)
#define MX_CRITICAL(...)   ::Mixture::Logging::Log::getClientLogger()->critical(__VA_ARGS__)

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