#include "Opal/Log.hpp"
#include "Opal/Colors.hpp"

#ifndef OPAL_PLATFORM_WINDOWS
#include <pthread.h>
#endif

namespace Opal {

    namespace
    {
        void AppendMarkerToBuffer(
            const spdlog::details::log_msg &msg,
            spdlog::memory_buf_t& dest,
            const char* colorFormatting,
            const char* colorReset
        ) {
            std::string logger_name(msg.logger_name.data(), msg.logger_name.size());
            size_t split_pos = logger_name.find('/');

            if (split_pos != std::string::npos)
            {
                std::string real_logger_name = logger_name.substr(0, split_pos);
                std::string marker_name = logger_name.substr(split_pos + 1);
                std::string output = real_logger_name + "/" + colorFormatting + marker_name + colorReset;

                dest.append(output.data(), output.data() + output.size());
            }
            else
            {
                // Fallback if no underscore is found (treat whole string as logger name)
                dest.append(logger_name.data(), logger_name.data() + logger_name.size());
            }
        }
    }

    /**
     * @brief Custom Spdlog flag formatter for colored markers.
     *
     * Parses the logger name and adds a colored suffix based on the name.
     */
    class ColorMarkerFlag : public spdlog::custom_flag_formatter
    {
    public:
        void format(const spdlog::details::log_msg &msg, const std::tm &, spdlog::memory_buf_t &dest) override
        {
            AppendMarkerToBuffer(msg, dest, Colors::magenta_bold.data(), Colors::reset.data());
        }

        std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<ColorMarkerFlag>();
        }
    };

    /**
     * @brief Custom Spdlog flag formatter for clean (non-colored) markers.
     *
     * Parses the logger name and adds a suffix based on the name, without ANSI colors.
     */
    class CleanMarkerFlag : public spdlog::custom_flag_formatter
    {
    public:
        void format(const spdlog::details::log_msg &msg, const std::tm &, spdlog::memory_buf_t &dest) override
        {
            AppendMarkerToBuffer(msg, dest, "", "");
        }

        std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<CleanMarkerFlag>();
        }
    };

    /**
     * @brief Custom Spdlog flag formatter for Thread Name.
     *
     * Retrieves the thread name via Opal::LogRegistry (thread_local) or OS API (pthread).
     */
    class ThreadNameFlag : public spdlog::custom_flag_formatter
    {
    public:
        void format(const spdlog::details::log_msg &, const std::tm &, spdlog::memory_buf_t &dest) override
        {
            // 1. Try Thread Local Cache (Fastest & Set by User)
            const std::string& name = Opal::LogRegistry::GetThreadName();
            if (!name.empty())
            {
                dest.append(name.data(), name.data() + name.size());
                return;
            }

            // 2. Try OS API
            char thread_name[32] = {0};

            #if defined(OPAL_PLATFORM_WINDOWS)
                // Windows implementation could go here (GetThreadDescription)
                // For now, default to empty to trigger fallback
            #elif defined(OPAL_PLATFORM_LINUX) || defined(OPAL_PLATFORM_DARWIN)
                pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name));
            #endif

            // 3. Fallback to Thread ID
            if (thread_name[0] == 0)
            {
                // Use default spdlog thread id formatting logic or just simple string
                 std::string tid = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
                 dest.append(tid.data(), tid.data() + tid.size());
            }
            else
            {
                dest.append(thread_name, thread_name + strlen(thread_name));
            }
        }

        std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<ThreadNameFlag>();
        }
    };

    class UppercaseLevelFlag : public spdlog::custom_flag_formatter
    {
    public:
        void format(const spdlog::details::log_msg &msg, const std::tm &, spdlog::memory_buf_t &dest) override
        {
            // Hardcode uppercase strings for performance
            // (Accessing msg.level which is an enum)
            const char* level_name = "";
            switch (msg.level)
            {
                case spdlog::level::trace:    level_name = "TRACE"; break;
                case spdlog::level::debug:    level_name = "DEBUG"; break;
                case spdlog::level::info:     level_name = "INFO";  break;
                case spdlog::level::warn:     level_name = "WARN";  break;
                case spdlog::level::err:      level_name = "ERROR"; break;
                case spdlog::level::critical: level_name = "CRITICAL"; break;
                case spdlog::level::off:      level_name = "OFF";   break;
                default:                      level_name = "UNKNOWN"; break;
            }

            // Helper to append string to the buffer
            spdlog::details::fmt_helper::append_string_view(level_name, dest);
        }

        std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<UppercaseLevelFlag>();
        }
    };
}
