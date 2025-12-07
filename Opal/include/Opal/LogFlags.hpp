#include "Opal/Log.hpp"

namespace Opal {

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
            std::string logger_name(msg.logger_name.data(), msg.logger_name.size());
            size_t split_pos = logger_name.find('_');

            if (split_pos != std::string::npos) {
                std::string marker = logger_name.substr(split_pos + 1);
                // ANSI Bold Magenta
                std::string output = "/\033[1;35m" + marker + "\033[0m"; 
                dest.append(output.data(), output.data() + output.size());
            }
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
            std::string logger_name(msg.logger_name.data(), msg.logger_name.size());
            size_t split_pos = logger_name.find('_');

            if (split_pos != std::string::npos) {
                std::string marker = logger_name.substr(split_pos + 1);
                // Just the slash and text, NO colors
                std::string output = "/" + marker; 
                dest.append(output.data(), output.data() + output.size());
            }
        }

        std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<CleanMarkerFlag>();
        }
    };

}