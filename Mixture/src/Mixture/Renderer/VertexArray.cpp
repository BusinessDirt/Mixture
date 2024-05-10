#include "mxpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Mixture/Platform/OpenGL/OpenGLVertexArray.h"

namespace Mixture {

	Ref<VertexArray> VertexArray::create() {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return createRef<OpenGLVertexArray>();
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}