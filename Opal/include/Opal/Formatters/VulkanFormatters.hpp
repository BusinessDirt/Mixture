#pragma once

#include <concepts>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp> 

#include <spdlog/fmt/fmt.h>

namespace fmt {
    // Concept to check if vk::to_string(T) exists
    template<typename T>
    concept HasVkToString = requires(T t) 
    {
        { vk::to_string(t) } -> std::convertible_to<std::string>;
    };

    // Formatter that catches ANY type satisfying the concept
    template <HasVkToString T>
    struct formatter<T> : formatter<std::string_view> 
    {
        auto format(const T& value, format_context& ctx) const -> decltype(ctx.out()) {
            return fmt::format_to(ctx.out(), "{}", vk::to_string(value));
        }
    };
}