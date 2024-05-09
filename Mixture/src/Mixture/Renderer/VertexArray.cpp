#include "mxpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Mixture/Platform/OpenGL/OpenGLVertexArray.h"

namespace Mixture {

	VertexArray* VertexArray::create() {
		switch (Renderer::getAPI()) {
			case RendererAPI::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::OpenGL:  return new OpenGLVertexArray();
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}