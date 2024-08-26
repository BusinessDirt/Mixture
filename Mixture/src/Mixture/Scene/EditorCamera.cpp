#include "mxpch.hpp"
#include "EditorCamera.hpp"

#include "Mixture/Input/Input.hpp"
#include "Mixture/Core/Timestep.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Mixture
{

    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
    {
        UpdateProjection();
        UpdateView();
    }

    void EditorCamera::UpdateProjection()
    {
        m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
        m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }

    void EditorCamera::UpdateView()
    {
        glm::quat orientation = glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));
        
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
        m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }

    void EditorCamera::OnUpdate(const Timestep& ts)
    {
        glm::vec3 direction = glm::vec3(0.0f);
        
        if (Input::IsKeyPressed(Key::W))
            direction.z -= 1.0f;
        if (Input::IsKeyPressed(Key::S))
            direction.z += 1.0f;
        if (Input::IsKeyPressed(Key::A))
            direction.x -= 1.0f;
        if (Input::IsKeyPressed(Key::D))
            direction.x += 1.0f;
        if (Input::IsKeyPressed(Key::Q))
            direction.y -= 1.0f;
        if (Input::IsKeyPressed(Key::E))
            direction.y += 1.0f;

        if (glm::length(direction) > 0.0f)
        {
            glm::quat orientation = glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));
            glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, 1.0f);
            glm::vec3 right = orientation * glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 up = orientation * glm::vec3(0.0f, -1.0f, 0.0f);

            float speed = 5.0f;
            m_Velocity = (forward * direction.z + right * direction.x + up * direction.y) * speed * (float)ts;

            m_Position += m_Velocity;
            UpdateView();
        }
    }

    bool EditorCamera::OnMouseMoved(MouseMovedEvent& e)
    {
        if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
        {
            float deltaX = e.GetX() - m_LastMouseX;
            float deltaY = m_LastMouseY - e.GetY();

            float sensitivity = 0.1f; // Adjust as needed

            m_Yaw -= deltaX * sensitivity;
            m_Pitch -= deltaY * sensitivity;

            // Clamp the pitch value to avoid gimbal lock
            m_Pitch = glm::clamp(m_Pitch, -glm::half_pi<float>(), glm::half_pi<float>());
            UpdateView();
        }
        
        m_LastMouseX = e.GetX();
        m_LastMouseY = e.GetY();
        
        return false;
    }

    void EditorCamera::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseMovedEvent>(MX_BIND_EVENT_FN(EditorCamera::OnMouseMoved));
    }
}
