#pragma once

#include "Mixture.hpp"

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
    };
}

