#include "mxpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Mixture/Platform/OpenGL/OpenGLShader.h"

namespace Mixture {
	Shader* Shader::create(const std::string& vertexSrc, const std::string& fragmentSrc) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None: MX_CORE_ASSERT(false, "RendererAPI:None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}