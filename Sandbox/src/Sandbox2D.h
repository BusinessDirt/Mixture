#pragma once

#include "Mixture.h"

class Sandbox2D : public Mixture::Layer {
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Mixture::Timestep ts) override;
	virtual void onImGuiRender() override;
	void onEvent(Mixture::Event& e) override;
private:
	Mixture::OrthographicCameraController m_CameraController;

	// Temp
	Mixture::Ref<Mixture::VertexArray> m_SquareVA;
	Mixture::Ref<Mixture::Shader> m_FlatColorShader;
	Mixture::Ref<Mixture::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};