#pragma once
#include <string_view>

namespace Opal::Colors 
{
#ifdef OPAL_PLATFORM_DARWIN
    // Disable colors for Darwin/macOS
    constexpr std::string_view reset = "";
    constexpr std::string_view bold = "";
    constexpr std::string_view dark = "";
    constexpr std::string_view underline = "";
    constexpr std::string_view blink = "";
    constexpr std::string_view reverse = "";
    constexpr std::string_view concealed = "";
    constexpr std::string_view clear_line = "";

    constexpr std::string_view black = "";
    constexpr std::string_view red = "";
    constexpr std::string_view green = "";
    constexpr std::string_view yellow = "";
    constexpr std::string_view blue = "";
    constexpr std::string_view magenta = "";
    constexpr std::string_view cyan = "";
    constexpr std::string_view white = "";

    constexpr std::string_view on_black = "";
    constexpr std::string_view on_red = "";
    constexpr std::string_view on_green = "";
    constexpr std::string_view on_yellow = "";
    constexpr std::string_view on_blue = "";
    constexpr std::string_view on_magenta = "";
    constexpr std::string_view on_cyan = "";
    constexpr std::string_view on_white = "";

    constexpr std::string_view yellow_bold = "";
    constexpr std::string_view red_bold = "";
    constexpr std::string_view magenta_bold = "";
    constexpr std::string_view bold_on_red = "";
#else
    // Text Attributes
    constexpr std::string_view reset = "\033[0m";
    constexpr std::string_view bold = "\033[1m";
    constexpr std::string_view dark = "\033[2m";
    constexpr std::string_view underline = "\033[4m";
    constexpr std::string_view blink = "\033[5m";
    constexpr std::string_view reverse = "\033[7m";
    constexpr std::string_view concealed = "\033[8m";
    constexpr std::string_view clear_line = "\033[K";

    // Foreground Colors
    constexpr std::string_view black = "\033[30m";
    constexpr std::string_view red = "\033[31m";
    constexpr std::string_view green = "\033[32m";
    constexpr std::string_view yellow = "\033[33m";
    constexpr std::string_view blue = "\033[34m";
    constexpr std::string_view magenta = "\033[35m";
    constexpr std::string_view cyan = "\033[36m";
    constexpr std::string_view white = "\033[37m";

    // Background Colors
    constexpr std::string_view on_black = "\033[40m";
    constexpr std::string_view on_red = "\033[41m";
    constexpr std::string_view on_green = "\033[42m";
    constexpr std::string_view on_yellow = "\033[43m";
    constexpr std::string_view on_blue = "\033[44m";
    constexpr std::string_view on_magenta = "\033[45m";
    constexpr std::string_view on_cyan = "\033[46m";
    constexpr std::string_view on_white = "\033[47m";

    // Combined Bold/Background Effects
    constexpr std::string_view yellow_bold = "\033[1;33m";
    constexpr std::string_view red_bold = "\033[1;31m";
    constexpr std::string_view magenta_bold = "\033[1;35m";
    constexpr std::string_view bold_on_red = "\033[1;41m";
#endif
}
