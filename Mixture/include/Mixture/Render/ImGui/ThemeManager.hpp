#pragma once

#include "Mixture/Render/ImGui/Theme.hpp"
#include "Mixture/Core/Base.hpp"

#include <vector>
#include <concepts>

namespace Mixture
{
    /**
     * @brief Singleton manager for registering and applying ImGui themes.
     */
    class ThemeManager
    {
    public:
        ThemeManager();
        ~ThemeManager() = default;

        /** Gets the ThemeManager singleton instance. */
        static ThemeManager& Get();

        /** Registers a theme by constructing it. */
        template<typename T, typename... Args>
            requires std::derived_from<T, ITheme>
        Ref<T> RegisterTheme(Args&&... args)
        {
            Ref<T> theme = CreateRef<T>(std::forward<Args>(args)...);
            RegisterTheme(theme);
            return theme;
        }

        /** Registers a theme instance. */
        void RegisterTheme(const Ref<ITheme>& theme);

        /** Sets the current active theme by name. */
        bool SetTheme(const std::string& name);

        /** Sets the current active theme instance. */
        void SetTheme(const Ref<ITheme>& theme);

        /** Gets the active theme. */
        OPAL_NODISCARD Ref<ITheme> GetActiveTheme() const { return m_ActiveTheme; }

        /** Gets all registered themes. */
        OPAL_NODISCARD const Vector<Ref<ITheme>>& GetThemes() const { return m_Themes; }

    private:
        Vector<Ref<ITheme>> m_Themes;
        Ref<ITheme> m_ActiveTheme;
    };
}
