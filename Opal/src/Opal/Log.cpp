#include "Opal/Log.hpp"
#include "Opal/LogFlags.hpp"

#include <filesystem>

#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

#ifdef OPAL_PLATFORM_WINDOWS
    #include <windows.h>
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif

    void PlatformEnableANSI() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
#endif

namespace Opal
{

    LogBuilder& LogBuilder::UseConsoleSink(spdlog::level::level_enum level)
    {
#ifdef OPAL_PLATFORM_WINDOWS
        PlatformEnableANSI();
#endif
        auto sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();

        // Set standard colors
        sink->set_color(spdlog::level::critical, sink->red_bold);
        sink->set_color(spdlog::level::err,      sink->red);
        sink->set_color(spdlog::level::warn,     sink->yellow);
        sink->set_color(spdlog::level::info,     sink->green);
        sink->set_color(spdlog::level::debug,    sink->blue);
        sink->set_color(spdlog::level::trace,    sink->white);

        // Apply Custom Formatter
        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<ColorMarkerFlag>('*')
                 .add_flag<ThreadNameFlag>('#')
                 .set_pattern("[%T] [%# at %^%l%$] (%*): %v");

        sink->set_formatter(std::move(formatter));
        sink->set_level(level);

        m_Sinks.push_back(sink);
        return *this;
    }

    LogBuilder& LogBuilder::UseFileSink(const std::string& filepath, spdlog::level::level_enum level)
    {
        auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filepath, 1024 * 1024 * 10, 10);
        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<CleanMarkerFlag>('*')
                 .add_flag<ThreadNameFlag>('#')
                 .set_pattern("[%T] [%# at %l] (%*): %v");

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

    void LogRegistry::SetThreadName(const std::string& name)
    {
#if defined(OPAL_PLATFORM_DARWIN)
        pthread_setname_np(name.c_str());
#elif defined(OPAL_PLATFORM_LINUX)
        pthread_setname_np(pthread_self(), name.c_str());
#elif defined(OPAL_PLATFORM_WINDOWS)
        // SetThreadDescription is available on Windows 10 1607+
        // Need conversion to wstring
        std::wstring wname(name.begin(), name.end());
        // Load library dynamically or assume win10 sdk usage
        // For simplicity, we can skip or use a simple hack if needed,
        // but user requested mainly for Darwin/Vulkan engine context.
        // SetThreadDescription(GetCurrentThread(), wname.c_str());
#endif
    }
}
