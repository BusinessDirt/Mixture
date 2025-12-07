#pragma once

#include <concepts>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp> 

#include <spdlog/fmt/fmt.h>

namespace fmt {
    /**
     * @brief Concept to check if vk::to_string(T) exists for a given type.
     */
    template<typename T>
    concept HasVkToString = requires(T t) 
    {
        { vk::to_string(t) } -> std::convertible_to<std::string>;
    };

    /**
     * @brief Formatter for Vulkan types that support vk::to_string.
     * 
     * Uses the vulkan.hpp to_string functionality to format Vulkan types.
     */
    template <HasVkToString T>
    struct formatter<T> : formatter<std::string_view> 
    {
        auto format(const T& value, format_context& ctx) const -> decltype(ctx.out()) {
            return fmt::format_to(ctx.out(), "{}", vk::to_string(value));
        }
    };
}