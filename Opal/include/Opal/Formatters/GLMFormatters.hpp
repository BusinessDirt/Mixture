#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <spdlog/fmt/fmt.h>

namespace fmt {

    /**
     * @brief Base formatter for GLM types.
     * 
     * Handles parsing of format specifiers (e.g., 'f' for fixed-point, 'e' for scientific).
     */
    struct GLMFormatter 
    {
		char presentation = 'f';

		// Parses standard format specifiers
		constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) 
        {
			auto it = ctx.begin(), end = ctx.end();
			if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;
			if (it != end && *it != '}') throw format_error("invalid format");
			return it;
		}
	};

    /*
    --- VECTORS ---
    */

    /**
     * @brief Formatter for glm::vec1.
     */
    template<>
	struct formatter<glm::vec1> : GLMFormatter
	{
		template <typename FormatContext>
		auto format(const glm::vec1& vec, FormatContext& ctx) const -> decltype(ctx.out())
		{
			return presentation == 'f'
				? fmt::format_to(ctx.out(), "({:.3f})", vec.x)
				: fmt::format_to(ctx.out(), "({:.3e})", vec.x);
		}
	};

    /**
     * @brief Formatter for glm::vec2.
     */
	template<>
	struct formatter<glm::vec2> : GLMFormatter
	{
		template <typename FormatContext>
		auto format(const glm::vec2& vec, FormatContext& ctx) const -> decltype(ctx.out())
		{
			return presentation == 'f'
				? fmt::format_to(ctx.out(), "({:.3f}, {:.3f})", vec.x, vec.y)
				: fmt::format_to(ctx.out(), "({:.3e}, {:.3e})", vec.x, vec.y);
		}
	};

    /**
     * @brief Formatter for glm::vec3.
     */
	template<>
	struct formatter<glm::vec3> : GLMFormatter
	{
		template <typename FormatContext>
		auto format(const glm::vec3& vec, FormatContext& ctx) const -> decltype(ctx.out())
		{
			return presentation == 'f'
				? fmt::format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f})", vec.x, vec.y, vec.z)
				: fmt::format_to(ctx.out(), "({:.3e}, {:.3e}, {:.3e})", vec.x, vec.y, vec.z);
		}
	};

    /**
     * @brief Formatter for glm::vec4.
     */
	template<>
	struct formatter<glm::vec4> : GLMFormatter
	{
		template <typename FormatContext>
		auto format(const glm::vec4& vec, FormatContext& ctx) const -> decltype(ctx.out())
		{
			return presentation == 'f'
				? fmt::format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f}, {:.3f})", vec.x, vec.y, vec.z, vec.w)
				: fmt::format_to(ctx.out(), "({:.3e}, {:.3e}, {:.3e}, {:.3e})", vec.x, vec.y, vec.z, vec.w);
		}
	};

    /*
    --- QUATERNIONS ---
    */

    /**
     * @brief Formatter for glm::quat.
     */
    template<> struct formatter<glm::quat> : GLMFormatter 
    {
        template <typename FormatContext>
        auto format(const glm::quat& q, FormatContext& ctx) const -> decltype(ctx.out()) 
        {
            // Standard notation: (w, x, y, z)
            return presentation == 'f'
                ? fmt::format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f}, {:.3f})", q.w, q.x, q.y, q.z)
                : fmt::format_to(ctx.out(), "({:.3e}, {:.3e}, {:.3e}, {:.3e})", q.w, q.x, q.y, q.z);
        }
    };

    /*
    --- MATRICES ---
    */

    /**
     * @brief Formatter for glm::mat2.
     */
    template<> struct formatter<glm::mat2> : GLMFormatter 
    {
        template <typename FormatContext>
        auto format(const glm::mat2& m, FormatContext& ctx) const -> decltype(ctx.out()) 
        {
            // GLM stores matrices in Column-Major order.
            // We can reuse the vec4 formatter to print the 4 columns.
            // Output format: [ Col1, Col2, Col3, Col4 ]
            
            // Note: We manually forward the presentation flag (f or e) to the children
            auto format_val = (presentation == 'f') ? "{:.3f}" : "{:.3e}";
            
            // Construct the string pattern: "[ {:f}, {:f}, {:f}, {:f} ]"
            // We let the vec4 formatter handle the heavy lifting.
            std::string pattern = fmt::format("[ {0}, {0} ]", 
                                              (presentation == 'f' ? "{:f}" : "{:e}"));

            return fmt::format_to(ctx.out(), fmt::runtime(pattern), m[0], m[1]);
        }
    };

    /**
     * @brief Formatter for glm::mat3.
     */
    template<> struct formatter<glm::mat3> : GLMFormatter 
    {
        template <typename FormatContext>
        auto format(const glm::mat3& m, FormatContext& ctx) const -> decltype(ctx.out()) 
        {
            // GLM stores matrices in Column-Major order.
            // We can reuse the vec4 formatter to print the 4 columns.
            // Output format: [ Col1, Col2, Col3, Col4 ]
            
            // Note: We manually forward the presentation flag (f or e) to the children
            auto format_val = (presentation == 'f') ? "{:.3f}" : "{:.3e}";
            
            // Construct the string pattern: "[ {:f}, {:f}, {:f}, {:f} ]"
            // We let the vec4 formatter handle the heavy lifting.
            std::string pattern = fmt::format("[ {0}, {0}, {0} ]", 
                                              (presentation == 'f' ? "{:f}" : "{:e}"));

            return fmt::format_to(ctx.out(), fmt::runtime(pattern), m[0], m[1], m[2]);
        }
    };

    /**
     * @brief Formatter for glm::mat4.
     */
    template<> struct formatter<glm::mat4> : GLMFormatter 
    {
        template <typename FormatContext>
        auto format(const glm::mat4& m, FormatContext& ctx) const -> decltype(ctx.out()) 
        {
            // GLM stores matrices in Column-Major order.
            // We can reuse the vec4 formatter to print the 4 columns.
            // Output format: [ Col1, Col2, Col3, Col4 ]
            
            // Note: We manually forward the presentation flag (f or e) to the children
            auto format_val = (presentation == 'f') ? "{:.3f}" : "{:.3e}";
            
            // Construct the string pattern: "[ {:f}, {:f}, {:f}, {:f} ]"
            // We let the vec4 formatter handle the heavy lifting.
            std::string pattern = fmt::format("[ {0}, {0}, {0}, {0} ]", 
                                              (presentation == 'f' ? "{:f}" : "{:e}"));

            return fmt::format_to(ctx.out(), fmt::runtime(pattern), m[0], m[1], m[2], m[3]);
        }
    };
}
