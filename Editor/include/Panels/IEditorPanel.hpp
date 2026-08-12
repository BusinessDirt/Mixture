#pragma once

#include "Mixture.hpp"

namespace Mixture
{
    /**
     * @brief Abstract base class for Editor UI panels.
     */
    class IEditorPanel
    {
    public:
        virtual ~IEditorPanel() = default;

        /** Called when the panel is attached/registered. */
        virtual void OnAttach() {}

        /** Called when the panel is detached/unregistered. */
        virtual void OnDetach() {}

        /** Called every frame to render the panel's ImGui interface. */
        virtual void OnDrawImGui() = 0;

        /** Called every frame to update panel logic. */
        virtual void OnUpdate(float dt) {}

        /** Called when an event is dispatched to this panel. */
        virtual void OnEvent(Event& event) {}

        /** Gets the display name of the panel. */
        OPAL_NODISCARD const std::string& GetName() const { return m_Name; }

        /** Returns whether the panel is currently open/visible. */
        OPAL_NODISCARD bool IsOpen() const { return m_IsOpen; }

        /** Sets the open/visible state of the panel. */
        void SetOpen(bool open) { m_IsOpen = open; }

    protected:
        explicit IEditorPanel(std::string name = "Panel", bool isOpen = true)
            : m_Name(std::move(name)), m_IsOpen(isOpen) {}

    protected:
        std::string m_Name;
        bool m_IsOpen = true;
    };
}
