#include "mxpch.h"
#include "Mixture/Input/OrthographicCameraController.h"

#include "Mixture/Input/Input.h"
#include "Mixture/Input/KeyCodes.h"

namespace Mixture {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
	{ }

	void OrthographicCameraController::onUpdate(Timestep ts) {
		MX_PROFILE_FUNCTION();

		if (Input::isKeyPressed(Key::A)) {
			m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		} else if (Input::isKeyPressed(Key::D)) {
			m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
			
		if (Input::isKeyPressed(Key::W)) {
			m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		} else if (Input::isKeyPressed(Key::S)) {
			m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}

		if (m_Rotation) {
			if (Input::isKeyPressed(Key::Q))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			if (Input::isKeyPressed(Key::E))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			if (m_CameraRotation < 180.0f)
				m_CameraRotation -= 360.0f;
			else if (m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;

			m_Camera.setRotation(m_CameraRotation);
		}

		m_Camera.setPosition(m_CameraPosition);

		m_CameraTranslationSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::onEvent(Event& e) {
		MX_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>(MX_BIND_EVENT_FN(OrthographicCameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizeEvent>(MX_BIND_EVENT_FN(OrthographicCameraController::onWindowResized));
	}

	void OrthographicCameraController::onResize(float width, float height) {
		m_AspectRatio = width / height;
		m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& e) {
		MX_PROFILE_FUNCTION();

		m_ZoomLevel -= e.getYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthographicCameraController::onWindowResized(WindowResizeEvent& e) {
		MX_PROFILE_FUNCTION();

		onResize((float)e.getWidth(), (float)e.getHeight());
		return false;
	}
}