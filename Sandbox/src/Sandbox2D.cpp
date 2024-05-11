#include "Sandbox2D.h"
#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f){}

void Sandbox2D::onAttach() {
	MX_PROFILE_FUNCTION();

	m_CheckerboardTexture = Mixture::Texture2D::create("assets/textures/Checkerboard.png");
}

void Sandbox2D::onDetach() {
	MX_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Mixture::Timestep ts) {
	MX_PROFILE_FUNCTION();

	// Update
	m_CameraController.onUpdate(ts);

	// Render
	{
		MX_PROFILE_SCOPE("Renderer Prep");
		Mixture::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Mixture::RenderCommand::clear();
	}

	{
		MX_PROFILE_SCOPE("Renderer Draw");
		Mixture::Renderer2D::beginScene(m_CameraController.getCamera());
		Mixture::Renderer2D::drawRotatedQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, glm::radians(-45.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
		Mixture::Renderer2D::drawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
		Mixture::Renderer2D::drawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);
		Mixture::Renderer2D::endScene();
	}
}

void Sandbox2D::onImGuiRender() {
	MX_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
}

void Sandbox2D::onEvent(Mixture::Event& e) {
	m_CameraController.onEvent(e);
}