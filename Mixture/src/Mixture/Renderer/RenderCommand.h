#pragma once

#include "Mixture/Renderer/RendererAPI.h"

namespace Mixture {

	class RenderCommand {
	public:
		static void init() {
			s_RendererAPI->init();
		}

		static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_RendererAPI->setViewport(x, y, width, height);
		}

		static void setClearColor(const glm::vec4& color) {
			s_RendererAPI->setClearColor(color);
		}

		static void clear() {
			s_RendererAPI->clear();
		}

		static void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0) {
			s_RendererAPI->drawIndexed(vertexArray, count);
		}
	private:
		static Scope<RendererAPI> s_RendererAPI;
	};

}