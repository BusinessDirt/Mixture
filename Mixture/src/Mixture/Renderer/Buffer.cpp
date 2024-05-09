#include "mxpch.h"
#include "Buffer.h"

#include "Mixture/Core.h"

#include "Renderer.h"

#include "Mixture/Platform/OpenGL/OpenGLBuffer.h"

namespace Mixture {

	VertexBuffer* VertexBuffer::create(float* vertices, uint32_t size) {
		switch (Renderer::getAPI()) {
			case RendererAPI::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::OpenGL:  return new OpenGLVertexBuffer(vertices, size);
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::create(uint32_t* indices, uint32_t size) {
		switch (Renderer::getAPI())
		{
			case RendererAPI::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::OpenGL:  return new OpenGLIndexBuffer(indices, size);
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}