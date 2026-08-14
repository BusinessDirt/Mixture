#pragma once

/**
 * @file MainLayer.hpp
 * @brief Primary application layer responsible for managing scene runtime and rendering scene entities.
 */

#include "Mixture.hpp"
#include "Mixture/Scene/Scene.hpp"

namespace Mixture
{

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
    };

    struct alignas(16) CameraData
    {
        glm::mat4 ViewProjection;
        glm::vec4 Position{ 0.0f };
    };

    constexpr uint32_t MaxSceneLights = 16;

    struct alignas(16) SceneLightData
    {
        glm::vec4 PositionRange;
        glm::vec4 DirectionType;
        glm::vec4 ColorIntensity;
        glm::vec4 SpotAngles;
    };

    struct alignas(16) SceneLightingData
    {
        glm::uvec4 Header{ 0u };
        std::array<SceneLightData, MaxSceneLights> Lights{};
    };

    static_assert(offsetof(SceneLightingData, Lights) == 16);

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
        Vector<Ref<RHI::IBuffer>> m_LightBuffers;
        uint32_t m_VertexCount = 0;
        uint32_t m_CameraBufferCursor = 0;
        uint32_t m_LightBufferCursor = 0;
        glm::mat4 m_LastCameraViewProjection{ 1.0f };
        SceneLightingData m_LastLightingData{};
        bool m_HasCameraData = false;
        bool m_HasLightingData = false;
    };
}
