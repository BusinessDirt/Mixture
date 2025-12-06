#include "Opal/Log.hpp"
#include "Opal/LogFlags.hpp"

#include <filesystem>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace Opal
{

    LogBuilder& LogBuilder::UseConsoleSink(spdlog::level::level_enum level)
    {
        auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        
        // Set standard colors
        sink->set_color(spdlog::level::critical, sink->red_bold);
        sink->set_color(spdlog::level::err,      sink->red);
        sink->set_color(spdlog::level::warn,     sink->yellow);
        sink->set_color(spdlog::level::info,     sink->green);
        sink->set_color(spdlog::level::debug,    sink->blue);
        sink->set_color(spdlog::level::trace,    sink->white);

        // Apply Custom Formatter
        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<ColorMarkerFlag>('*').set_pattern("[%T] [Thread %t at %^%l%$] (%n%*): %v");
        
        sink->set_formatter(std::move(formatter));
        sink->set_level(level);
        
        m_Sinks.push_back(sink);
        return *this;
    }

    LogBuilder& LogBuilder::UseFileSink(const std::string& filepath, spdlog::level::level_enum level) 
    {
        auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filepath, 1024 * 1024 * 10, 10);
        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<CleanMarkerFlag>('*').set_pattern("[%T] [Thread %t at %l] (%n%*): %v");

        sink->set_formatter(std::move(formatter));
        sink->set_level(level);

        m_Sinks.push_back(sink);
        return *this;
    }

    std::shared_ptr<spdlog::logger> LogRegistry::GetLogger(const std::string& name) 
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        // Check Cache
        auto it = m_Loggers.find(name);
        if (it != m_Loggers.end()) return it->second;

        auto newLogger = std::make_shared<spdlog::logger>(name, begin(m_Sinks), end(m_Sinks));
        
        // Default settings for new loggers
        newLogger->set_level(spdlog::level::trace);
        newLogger->flush_on(spdlog::level::trace);
        
        // Register with SPDLog global registry (optional, but good practice)
        spdlog::register_logger(newLogger);

        // Cache it
        m_Loggers[name] = newLogger;
        return newLogger;
    }
}
