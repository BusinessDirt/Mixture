#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/bundled/format.h>
#pragma warning(pop)

namespace Opal
{
    class Log
    {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

#ifndef OPAL_DIST

// Core log macros
#define OPAL_CORE_TRACE(...)     ::Opal::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define OPAL_CORE_INFO(...)      ::Opal::Log::GetCoreLogger()->info(__VA_ARGS__)
#define OPAL_CORE_WARN(...)      ::Opal::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define OPAL_CORE_ERROR(...)     ::Opal::Log::GetCoreLogger()->error(__VA_ARGS__)
#define OPAL_CORE_CRITICAL(...)     ::Opal::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define OPAL_TRACE(...)   ::Opal::Log::GetClientLogger()->trace(__VA_ARGS__)
#define OPAL_INFO(...)    ::Opal::Log::GetClientLogger()->info(__VA_ARGS__)
#define OPAL_WARN(...)    ::Opal::Log::GetClientLogger()->warn(__VA_ARGS__)
#define OPAL_ERROR(...)   ::Opal::Log::GetClientLogger()->error(__VA_ARGS__)
#define OPAL_CRITICAL(...)   ::Opal::Log::GetClientLogger()->critical(__VA_ARGS__)

#else // strip logging in dist build

// Core log macros
#define OPAL_CORE_TRACE(...)
#define OPAL_CORE_INFO(...)
#define OPAL_CORE_WARN(...)
#define OPAL_CORE_ERROR(...)
#define OPAL_CORE_FATAL(...)

// Client log macros
#define OPAL_TRACE(...)
#define OPAL_INFO(...)
#define OPAL_WARN(...)
#define OPAL_ERROR(...)
#define OPAL_FATAL(...)

#endif
