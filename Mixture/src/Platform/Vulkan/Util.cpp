#include "mxpch.hpp"
#include "Platform/Vulkan/Util.hpp"

std::optional<uint32_t> Mixture::Vulkan::ParseVulkanVersion(std::string_view value)
{
    uint32_t major = 0;
    uint32_t minor = 0;
    uint32_t patch = 0;

    const auto firstDot = value.find('.');
    const auto secondDot = value.find('.', firstDot + 1);

    if (firstDot == std::string_view::npos ||
        secondDot == std::string_view::npos)
    {
        return std::nullopt;
    }

    const auto parse = [](std::string_view text, uint32_t& result)
    {
        const auto [end, error] =
            std::from_chars(text.data(), text.data() + text.size(), result);

        return error == std::errc{} &&
            end == text.data() + text.size();
    };

    if (!parse(value.substr(0, firstDot), major) ||
        !parse(value.substr(firstDot + 1, secondDot - firstDot - 1), minor) ||
        !parse(value.substr(secondDot + 1), patch))
    {
        return std::nullopt;
    }

    return VK_MAKE_VERSION(major, minor, patch);
}
