#pragma once

/**
 * @file RHI.hpp
 * @brief Main header for the Render Hardware Interface (RHI).
 *
 * Includes all RHI interfaces and definitions.
 */

#include "Mixture/Render/RHI/RenderFormats.hpp"
#include "Mixture/Render/RHI/RenderStates.hpp"
#include "Mixture/Render/RHI/ResourceStates.hpp"

#include "Mixture/Render/RHI/ITexture.hpp"
#include "Mixture/Render/RHI/IBuffer.hpp"
#include "Mixture/Render/RHI/IPipeline.hpp"

#include "Mixture/Render/RHI/IGraphicsContext.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"
#include "Mixture/Render/RHI/ICommandList.hpp"

#include <concepts>

#include <spdlog/fmt/fmt.h>

namespace fmt
{
    /**
     * @brief Concept to check if RHI::ToString(T) exists for a given type.
     */
    template<typename T>
    concept HasRHIToString = requires(T t)
    {
        { Mixture::RHI::ToString(t) } -> std::convertible_to<std::string_view>;
    };

    /**
     * @brief Formatter for RHI types that support RHI::ToString.
     *
     * Uses the RHI::ToString functionality to format RHI types.
     */
    template <HasRHIToString T>
    struct formatter<T> : formatter<std::string_view>
    {
        auto format(const T& value, format_context& ctx) const -> decltype(ctx.out())
        {
            return fmt::format_to(ctx.out(), "{}", Mixture::RHI::ToString(value));
        }
    };
}
