#pragma once

#include "Mixture/Renderer/OrthographicCamera.h"

#include "Mixture/Renderer/Texture.h"

namespace Mixture {
	class Renderer2D {
	public:
		static void init();
		static void shutdown();

		static void beginScene(const OrthographicCamera& camera);
		static void endScene();

		// primitives
		static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		static void drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture>& texture, 
			float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture>& texture,
			float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);

		static void drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture>& texture,
			float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture>& texture,
			float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

	private:
		inline static void drawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture>& texture,
			float tilingFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
	};
}