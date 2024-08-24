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
        Scope<UniformBuffer> m_GlobalUbo;
        float m_FrameTime = 0.0f;
        
        // Scene
        EditorCamera m_Camera;
        Scope<Model> m_Model;
        Scope<Texture> m_Texture;
    };
}
