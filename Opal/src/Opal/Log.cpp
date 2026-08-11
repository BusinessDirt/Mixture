#include "Opal/Log.hpp"
#include "Opal/LogFlags.hpp"

#include <thread>

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
    LogBuilder::LogBuilder()
        : m_FileResolver(ILogFileResolver::Create())
    {
    }

    LogBuilder& LogBuilder::UseFileResolver(std::shared_ptr<ILogFileResolver> resolver)
    {
        if (!resolver) throw std::invalid_argument("Log file resolver must not be null");
        m_FileResolver = std::move(resolver);
        return *this;
    }

    void LogRegistry::Initialize(const std::vector<spdlog::sink_ptr>& sinks)
    {
        std::unique_lock lock(m_Mutex);
        m_Sinks = sinks;
    }

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
                 .add_flag<UppercaseLevelFlag>('U')
                 .set_pattern("[%T] [%#/%^%U%$] (%*): %v");

        sink->set_formatter(std::move(formatter));
        sink->set_level(level);

        m_Sinks.push_back(sink);
        return *this;
    }

    LogBuilder& LogBuilder::UseFileSink(const std::string& filename, spdlog::level::level_enum level)
    {
        auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(m_FileResolver->Resolve(filename).string(), 1024 * 1024 * 10, 10);
        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<CleanMarkerFlag>('*')
                 .add_flag<ThreadNameFlag>('#')
                 .add_flag<UppercaseLevelFlag>('U')
                 .set_pattern("[%T] [%#/%U] (%*): %v");

        sink->set_formatter(std::move(formatter));
        sink->set_level(level);

        m_Sinks.push_back(sink);
        return *this;
    }

    std::shared_ptr<spdlog::logger> LogRegistry::GetLogger(const std::string& name)
    {
        struct CachedLogger
        {
            const LogRegistry* Registry = nullptr;
            std::string Name;
            std::weak_ptr<spdlog::logger> Logger;
        };
        static thread_local CachedLogger cache;

        if (cache.Registry == this && cache.Name == name)
        {
            if (auto logger = cache.Logger.lock()) return logger;
        }

        m_RegistryLookupCount.fetch_add(1, std::memory_order_relaxed);
        {
            std::shared_lock lock(m_Mutex);
            auto it = m_Loggers.find(name);
            if (it != m_Loggers.end())
            {
                cache = { this, name, it->second };
                return it->second;
            }
        }

        std::unique_lock lock(m_Mutex);

        auto it = m_Loggers.find(name);
        if (it != m_Loggers.end())
        {
            cache = { this, name, it->second };
            return it->second;
        }

        auto newLogger = std::make_shared<spdlog::logger>(name, begin(m_Sinks), end(m_Sinks));

        // Default settings for new loggers
        newLogger->set_level(spdlog::level::trace);
        newLogger->flush_on(spdlog::level::err);

        // Register with SPDLog global registry (optional, but good practice)
        spdlog::register_logger(newLogger);

        // Cache it
        m_Loggers[name] = newLogger;
        cache = { this, name, newLogger };
        return newLogger;
    }

    void LogRegistry::FlushAll()
    {
        std::shared_lock lock(m_Mutex);
        for (const auto& [name, logger] : m_Loggers)
        {
            (void)name;
            logger->flush();
        }
    }

    static thread_local std::string s_CurrentThreadName;

    void LogRegistry::SetThreadName(const std::string& name)
    {
        s_CurrentThreadName = name;

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

    const std::string& LogRegistry::GetThreadName()
    {
        return s_CurrentThreadName;
    }
}
