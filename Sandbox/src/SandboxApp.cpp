#include "Mixture.h"

#include "Mixture/Platform/OpenGL/OpenGLShader.h"

#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Mixture::Window;

class ExampleLayer : public Mixture::Layer {
public:
	ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f) {

		m_VertexArray.reset(Mixture::VertexArray::create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		Mixture::Ref<Mixture::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Mixture::VertexBuffer::create(vertices, sizeof(vertices)));
		Mixture::BufferLayout layout = {
			{ Mixture::ShaderDataType::Float3, "a_Position" },
			{ Mixture::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->setLayout(layout);
		m_VertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		Mixture::Ref<Mixture::IndexBuffer> indexBuffer;
		indexBuffer.reset(Mixture::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->setIndexBuffer(indexBuffer);

		m_SquareVA.reset(Mixture::VertexArray::create());

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Mixture::Ref<Mixture::VertexBuffer> squareVB;
		squareVB.reset(Mixture::VertexBuffer::create(squareVertices, sizeof(squareVertices)));
		squareVB->setLayout({
			{ Mixture::ShaderDataType::Float3, "a_Position" },
			{ Mixture::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SquareVA->addVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Mixture::Ref<Mixture::IndexBuffer> squareIB;
		squareIB.reset(Mixture::IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->setIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader = Mixture::Shader::create("VertexPosColor", vertexSrc, fragmentSrc);

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			uniform vec3 u_Color;
			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader = Mixture::Shader::create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);
		Mixture::Ref<Mixture::Shader> textureShader = m_ShaderLibrary.load("assets/shaders/Texture.glsl");

		m_Texture = Mixture::Texture2D::create("assets/textures/Checkerboard.png");
		m_LogoTexture = Mixture::Texture2D::create("assets/textures/Logo.png");

		std::dynamic_pointer_cast<Mixture::OpenGLShader>(textureShader)->bind();
		std::dynamic_pointer_cast<Mixture::OpenGLShader>(textureShader)->uploadUniformInt("u_Texture", 0);
	}

	void onUpdate(Mixture::Timestep ts) override {
		if (Mixture::Input::isKeyPressed(MX_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		else if (Mixture::Input::isKeyPressed(MX_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;

		if (Mixture::Input::isKeyPressed(MX_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		else if (Mixture::Input::isKeyPressed(MX_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if (Mixture::Input::isKeyPressed(MX_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		if (Mixture::Input::isKeyPressed(MX_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed * ts;

		Mixture::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Mixture::RenderCommand::clear();

		m_Camera.setPosition(m_CameraPosition);
		m_Camera.setRotation(m_CameraRotation);

		Mixture::Renderer::beginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Mixture::OpenGLShader>(m_FlatColorShader)->bind();
		std::dynamic_pointer_cast<Mixture::OpenGLShader>(m_FlatColorShader)->uploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++) {
			for (int x = 0; x < 20; x++) {
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Mixture::Renderer::submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		Mixture::Ref<Mixture::Shader> textureShader = m_ShaderLibrary.get("Texture");

		m_Texture->bind();
		Mixture::Renderer::submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_LogoTexture->bind();
		Mixture::Renderer::submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		Mixture::Renderer::endScene();
	}

	virtual void onImGuiRender() override {
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void onEvent(Mixture::Event& event) override {
		
	}
private:
	Mixture::ShaderLibrary m_ShaderLibrary;
	Mixture::Ref<Mixture::Shader> m_Shader;
	Mixture::Ref<Mixture::VertexArray> m_VertexArray;

	Mixture::Ref<Mixture::Shader> m_FlatColorShader;
	Mixture::Ref<Mixture::VertexArray> m_SquareVA;

	Mixture::Ref<Mixture::Texture> m_Texture, m_LogoTexture;

	Mixture::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};

class Sandbox : public Mixture::Application {
public:
	Sandbox() {
		pushLayer(new ExampleLayer());
	}

	~Sandbox() {

	}
};

Mixture::Application* Mixture::CreateApplication() {
	return new Sandbox();
}