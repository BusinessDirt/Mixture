#pragma once

#include "Mixture/Core/Base.hpp"
#include <string>

namespace Mixture
{
    /**
     * @brief Abstract interface for an ImGui UI theme and style configuration.
     */
    class ITheme
    {
    public:
        virtual ~ITheme() = default;

        /** Gets the display name of the theme. */
        OPAL_NODISCARD virtual const std::string& GetName() const = 0;

        /** Applies the theme styling and colors to ImGui. */
        virtual void Apply() = 0;
    };

    /**
     * @brief Modern Dark Engine Theme.
     */
    class DarkTheme : public ITheme
    {
    public:
        DarkTheme() = default;
        ~DarkTheme() override = default;

        OPAL_NODISCARD const std::string& GetName() const override { return m_Name; }
        void Apply() override;

    private:
        std::string m_Name = "Dark Theme";
    };

    /**
     * @brief Clean Light Engine Theme.
     */
    class LightTheme : public ITheme
    {
    public:
        LightTheme() = default;
        ~LightTheme() override = default;

        OPAL_NODISCARD const std::string& GetName() const override { return m_Name; }
        void Apply() override;

    private:
        std::string m_Name = "Light Theme";
    };
}
