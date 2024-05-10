#include "mxpch.h"
#include "Mixture/Renderer/Renderer.h"
#include "Mixture/Renderer/Renderer2D.h"

#include "Mixture/Platform/OpenGL/OpenGLShader.h"
#include "Renderer2D.h"

namespace Mixture {

	Scope<Renderer::SceneData> Renderer::s_SceneData = createScope<Renderer::SceneData>();

	void Renderer::init() {
		RenderCommand::init();
		Renderer2D::init();
	}

	void Renderer::shutdown() {
		Renderer2D::shutdown();
	}

	void Renderer::onWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::setViewport(0, 0, width, height);
	}

	void Renderer::beginScene(OrthographicCamera& camera) {
		s_SceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::endScene() {
	}

	void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray,
		const glm::mat4& transform) {
		shader->bind();
		shader->setMat4("u_ViewProjection", s_SceneData->viewProjectionMatrix);
		shader->setMat4("u_Transform", transform);

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}