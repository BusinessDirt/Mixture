#include "mxpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include "Mixture/Platform/OpenGL/OpenGLShader.h"

namespace Mixture {
	struct Renderer2DStorage {
		Ref<VertexArray> quadVertexArray;
		Ref<Shader> flatColorShader;
	};

	static Scope<Renderer2DStorage> s_Data;

	void Renderer2D::init() {
		s_Data = createScope<Renderer2DStorage>();
		s_Data->quadVertexArray = VertexArray::create();

		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		Ref<VertexBuffer> squareVB;
		squareVB = VertexBuffer::create(squareVertices, sizeof(squareVertices));
		squareVB->setLayout({
			{ ShaderDataType::Float3, "a_Position" }
		});
		s_Data->quadVertexArray->addVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB;
		squareIB = IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		s_Data->quadVertexArray->setIndexBuffer(squareIB);

		s_Data->flatColorShader = Shader::create("assets/shaders/FlatColor.glsl");
	}

	void Renderer2D::shutdown() {
		s_Data.release();
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera) {
		std::dynamic_pointer_cast<OpenGLShader>(s_Data->flatColorShader)->bind();
		std::dynamic_pointer_cast<OpenGLShader>(s_Data->flatColorShader)->uploadUniformMat4("u_ViewProjection", camera.getViewProjectionMatrix());
		std::dynamic_pointer_cast<OpenGLShader>(s_Data->flatColorShader)->uploadUniformMat4("u_Transform", glm::mat4(1.0f));
	}

	void Renderer2D::endScene() {
	}
	
	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
		drawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
		std::dynamic_pointer_cast<OpenGLShader>(s_Data->flatColorShader)->bind();
		std::dynamic_pointer_cast<OpenGLShader>(s_Data->flatColorShader)->uploadUniformFloat4("u_Color", color);

		s_Data->quadVertexArray->bind();
		RenderCommand::drawIndexed(s_Data->quadVertexArray);
	}
}