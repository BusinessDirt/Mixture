#pragma once

#include "Mixture.hpp"

namespace Mixture
{
    class MainLayer : public Layer
    {
    public:
        MainLayer() : Layer("MainLayer") {}
        ~MainLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(FrameInfo& frameInfo) override;
        void OnUIRender() override;
        void OnEvent(Event& event) override;
        
    private:
        Scope<GraphicsPipeline> m_Pipeline;
        Scope<VertexBuffer> m_VertexBuffer;
        Scope<IndexBuffer> m_IndexBuffer;
        Scope<UniformBuffer> m_UniformBuffer;
        Scope<Texture> m_Texture;
    };
}
