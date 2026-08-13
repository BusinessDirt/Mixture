#pragma once

/**
 * @file MainLayer.hpp
 * @brief Primary application layer responsible for managing scene runtime and rendering scene entities.
 */

#include "Mixture.hpp"
#include "Mixture/Scene/Scene.hpp"

namespace Mixture
{
    /**
     * @brief Main layer managing 3D scene instance and scene render graph passes.
     */
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

        /** Gets reference to active scene instance. */
        OPAL_NODISCARD Ref<Scene> GetScene() const { return m_Scene; }

    private:
        Ref<Scene> m_Scene;
        Ref<RHI::IBuffer> m_VertexBuffer;
        Vector<Ref<RHI::IBuffer>> m_CameraBuffers;
        uint32_t m_VertexCount = 0;
        uint32_t m_CameraBufferCursor = 0;
        glm::mat4 m_LastCameraViewProjection{ 1.0f };
        bool m_HasCameraData = false;
    };
}
