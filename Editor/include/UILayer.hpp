#pragma once

#include "Mixture.hpp"
#include "Panels/IEditorPanel.hpp"

#include <concepts>

namespace Mixture
{
    class UILayer final : public Layer
    {
    public:
        OPAL_NON_COPIABLE(UILayer);
        UILayer() : Layer("UILayer") {}
        ~UILayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        
        void OnEvent(Event& event) override;

        void OnUpdate(float dt) override;
        void OnRender(RenderGraph& graph) override;
        void OnDrawImGui() override;

        /**
         * @brief Reset dockspace to the default window layout.
         */
        void ResetLayout() { m_ResetLayoutRequested = true; }

        /**
         * @brief Register a panel by constructing it directly.
         */
        template<typename T, typename... Args>
            requires std::derived_from<T, IEditorPanel>
        Ref<T> RegisterPanel(Args&&... args)
        {
            Ref<T> panel = CreateRef<T>(std::forward<Args>(args)...);
            RegisterPanel(panel);
            return panel;
        }

        /**
         * @brief Register an existing panel instance.
         */
        void RegisterPanel(const Ref<IEditorPanel>& panel);

        /**
         * @brief Unregister a panel instance.
         */
        void UnregisterPanel(const Ref<IEditorPanel>& panel);

        /**
         * @brief Get a registered panel by type.
         */
        template<typename T>
            requires std::derived_from<T, IEditorPanel>
        Ref<T> GetPanel() const
        {
            for (const auto& panel : m_Panels)
            {
                if (auto casted = std::dynamic_pointer_cast<T>(panel))
                {
                    return casted;
                }
            }
            return nullptr;
        }

        /**
         * @brief Gets all registered panels.
         */
        OPAL_NODISCARD const Vector<Ref<IEditorPanel>>& GetPanels() const { return m_Panels; }

    private:
        void SetupDefaultDockLayout(unsigned int dockspaceId);

    private:
        Vector<Ref<IEditorPanel>> m_Panels;
        bool m_IsAttached = false;
        bool m_ResetLayoutRequested = false;
    };
}
