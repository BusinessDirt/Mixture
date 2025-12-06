#pragma once

#include "Opal/Base.hpp"
#include "Opal/Formatters/LogFormatters.hpp"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/fmt/bundled/format.h>
#pragma warning(pop)

namespace Opal
{
    class LogBuilder 
    {
    public:
        LogBuilder() = default;

        LogBuilder& UseConsoleSink(spdlog::level::level_enum level = spdlog::level::trace);
        LogBuilder& UseFileSink(const std::string& filepath, spdlog::level::level_enum level = spdlog::level::trace);
        LogBuilder& AddSink(std::shared_ptr<spdlog::sinks::sink> sink) 
        {
            m_Sinks.push_back(sink);
            return *this;
        }

        std::vector<spdlog::sink_ptr> Build() const { return m_Sinks; }

    private:
        std::vector<spdlog::sink_ptr> m_Sinks;
    };

    class LogRegistry 
    {
    public:
        static LogRegistry& Get() 
        {
            static LogRegistry instance;
            return instance;
        }

        void Initialize(const std::vector<spdlog::sink_ptr>& sinks) { m_Sinks = sinks; }

        std::shared_ptr<spdlog::logger> GetLogger(const std::string& name);

    private:
        LogRegistry() = default; // Singleton
        std::vector<spdlog::sink_ptr> m_Sinks;
        std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> m_Loggers;
        std::mutex m_Mutex;
    };
}

#ifndef OPAL_DIST

// Core log macros
#define OPAL_TRACE(logger, ...)     ::Opal::LogRegistry::Get().GetLogger(logger)->trace(__VA_ARGS__)
#define OPAL_INFO(logger, ...)      ::Opal::LogRegistry::Get().GetLogger(logger)->info(__VA_ARGS__)
#define OPAL_WARN(logger, ...)      ::Opal::LogRegistry::Get().GetLogger(logger)->warn(__VA_ARGS__)
#define OPAL_ERROR(logger, ...)     ::Opal::LogRegistry::Get().GetLogger(logger)->error(__VA_ARGS__)
#define OPAL_CRITICAL(logger, ...)     ::Opal::LogRegistry::Get().GetLogger(logger)->critical(__VA_ARGS__)

#else // strip logging in dist build

// Core log macros
#define OPAL_TRACE(...)
#define OPAL_INFO(...)
#define OPAL_WARN(...)
#define OPAL_ERROR(...)
#define OPAL_FATAL(...)

#endif
