#include "Opal/Log.hpp"

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
            AppendMarkerToBuffer(msg, dest, ANSI_MAGENTA_BOLD, ANSI_RESET);
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

}
