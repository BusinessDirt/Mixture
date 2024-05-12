#pragma once

#include "Mixture/Renderer/Camera.h"

namespace Mixture {
	class SceneCamera : public Camera {
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void setOrthographic(float size, float nearClip, float farClip);

		void setViewportSize(uint32_t width, uint32_t height);

		float getOrthographicSize() const { return m_OrthographicSize; }
		void setOrthographicSize(float size) { 
			m_OrthographicSize = size;
			recalculateProjection();
		}
	private:
		void recalculateProjection();
	private:
		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		float m_AspectRatio = 0.0f;
	};
}