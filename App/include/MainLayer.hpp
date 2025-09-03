#pragma once

#include "Mixture.hpp"

namespace Mixture
{
    struct UniformBufferObject
    {
        glm::mat4 View;
        glm::mat4 Projection;
    };

    class MainLayer final : public Layer
    {
    public:
        OPAL_NON_COPIABLE(MainLayer);
        MainLayer() : Layer("MainLayer") {}
        ~MainLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        
        void OnUpdate(FrameInfo& frameInfo) override;
        void OnRender(FrameInfo& frameInfo) override;
        void OnRenderImGui(FrameInfo& frameInfo) override;
        
        void OnEvent(Event& event) override;
        
    private:
        Ref<GraphicsPipeline> m_GraphicsPipeline;
        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;
        
        Ref<UniformBuffer> m_UniformBuffer;
        UniformBufferObject m_UniformBufferObject{};
        
        float m_Z = 1.0f;
        
        bool m_ViewportFocused = false;
    };
}

