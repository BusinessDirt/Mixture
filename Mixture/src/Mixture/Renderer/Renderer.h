#pragma once

#include "Mixture/Renderer/RenderCommand.h"

#include "Mixture/Renderer/OrthographicCamera.h"
#include "Mixture/Renderer/Shader.h"

namespace Mixture {
	class Renderer
	{
	public:
		static void init();
		static void shutdown();

		static void onWindowResize(uint32_t width, uint32_t height);

		static void beginScene(OrthographicCamera& camera);
		static void endScene();

		static void submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform);

		static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	private:
		struct SceneData {
			glm::mat4 viewProjectionMatrix;
		};

		static Scope<SceneData> s_SceneData;
	};
}