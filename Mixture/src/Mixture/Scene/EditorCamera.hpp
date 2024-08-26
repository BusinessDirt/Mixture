#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Core/Timestep.hpp"
#include "Mixture/Events/MouseEvent.hpp"
#include "Mixture/Scene/Camera.hpp"

namespace Mixture
{
    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);
        
    public:
        void OnUpdate(const Timestep& ts);
        void OnEvent(Event& e);
        
        inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }
        
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }
    private:
        void UpdateProjection();
        void UpdateView();
        
        bool OnMouseMoved(MouseMovedEvent& e);
    private:
        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;
        
        glm::mat4 m_ViewMatrix;
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_Velocity = { 0.0f, 0.0f, 0.0f };
        float m_Pitch = 0.0f, m_Yaw = 0.0f;

        float m_ViewportWidth = 1280, m_ViewportHeight = 720;
        float m_LastMouseX = 0.0f, m_LastMouseY = 0.0f;
    };
}
