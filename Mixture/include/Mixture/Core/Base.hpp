#pragma once

/**
 * @file Base.hpp
 * @brief Base definitions, types, and macros for the Mixture engine.
 */

#include <Opal/Base.hpp>

namespace Mixture
{

    /**
     * @brief Scoped pointer alias (std::unique_ptr).
     */
    template<typename T>
    using Scope = std::unique_ptr<T>;

    /**
     * @brief Creates a scoped pointer.
     *
     * @tparam T Type of the object.
     * @tparam Args Argument types for the constructor.
     * @param args Arguments for the constructor.
     * @return Scope<T> The created scoped pointer.
     */
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief Shared pointer alias (std::shared_ptr).
     */
    template<typename T>
    using Ref = std::shared_ptr<T>;

    /**
     * @brief Creates a shared pointer.
     *
     * @tparam T Type of the object.
     * @tparam Args Argument types for the constructor.
     * @param args Arguments for the constructor.
     * @return Ref<T> The created shared pointer.
     */
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief Vector alias (std::vector).
     */
    template<typename T>
    using Vector = std::vector<T>;
}
