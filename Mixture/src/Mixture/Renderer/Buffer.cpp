#include "mxpch.h"
#include "Buffer.h"

#include "Mixture/Core/Core.h"

#include "Renderer.h"

#include "Mixture/Platform/OpenGL/OpenGLBuffer.h"

namespace Mixture {

	Ref<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return createRef<OpenGLVertexBuffer>(vertices, size);
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t size) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return createRef<OpenGLIndexBuffer>(indices, size);
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}