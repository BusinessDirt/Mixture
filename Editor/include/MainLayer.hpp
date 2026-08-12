#pragma once

#include "Mixture.hpp"

namespace Mixture
{
    class MainLayer final : public Layer
    {
    public:
        OPAL_NON_COPIABLE(MainLayer);
        MainLayer() : Layer("MainLayer") {}
        ~MainLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        
        void OnEvent(Event& event) override;

        void OnUpdate(float dt) override;
        void OnRender(RenderGraph& graph) override;

    private:
        Ref<RHI::IBuffer> m_VertexBuffer;
    };
}

