#include "mxpch.h"
#include "Mixture/Renderer/GraphicsContext.h"

#include "Mixture/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Mixture {
	Scope<GraphicsContext> GraphicsContext::create(void* window) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None: MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return createScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}