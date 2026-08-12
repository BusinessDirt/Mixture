#include "mxpch.hpp"
#include "Mixture/Render/ImGui/ThemeManager.hpp"

#include <algorithm>

namespace Mixture
{
    ThemeManager::ThemeManager()
    {
        auto dark = RegisterTheme<DarkTheme>();
        RegisterTheme<LightTheme>();
        SetTheme(dark);
    }

    ThemeManager& ThemeManager::Get()
    {
        static ThemeManager instance;
        return instance;
    }

    void ThemeManager::RegisterTheme(const Ref<ITheme>& theme)
    {
        if (!theme) return;

        auto it = std::find(m_Themes.begin(), m_Themes.end(), theme);
        if (it == m_Themes.end())
        {
            m_Themes.push_back(theme);
        }
    }

    bool ThemeManager::SetTheme(const std::string& name)
    {
        for (const auto& theme : m_Themes)
        {
            if (theme && theme->GetName() == name)
            {
                SetTheme(theme);
                return true;
            }
        }
        return false;
    }

    void ThemeManager::SetTheme(const Ref<ITheme>& theme)
    {
        if (!theme) return;

        m_ActiveTheme = theme;
        m_ActiveTheme->Apply();
    }
}
