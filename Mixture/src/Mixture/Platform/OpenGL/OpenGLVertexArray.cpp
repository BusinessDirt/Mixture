#include "mxpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Mixture {

	static GLenum shaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type)
		{
			case Mixture::ShaderDataType::Float:    return GL_FLOAT;
			case Mixture::ShaderDataType::Float2:   return GL_FLOAT;
			case Mixture::ShaderDataType::Float3:   return GL_FLOAT;
			case Mixture::ShaderDataType::Float4:   return GL_FLOAT;
			case Mixture::ShaderDataType::Mat3:     return GL_FLOAT;
			case Mixture::ShaderDataType::Mat4:     return GL_FLOAT;
			case Mixture::ShaderDataType::Int:      return GL_INT;
			case Mixture::ShaderDataType::Int2:     return GL_INT;
			case Mixture::ShaderDataType::Int3:     return GL_INT;
			case Mixture::ShaderDataType::Int4:     return GL_INT;
			case Mixture::ShaderDataType::Bool:     return GL_BOOL;
		}

		MX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::bind() const {
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::unbind() const {
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
		MX_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->bind();

		const BufferLayout& layout = vertexBuffer->getLayout();
		for (const auto& element : layout) {
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer(m_VertexBufferIndex, element.getComponentCount(), shaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE, layout.getStride(), (const void*)(intptr_t)element.offset);
			m_VertexBufferIndex++;
		}
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
		glBindVertexArray(m_RendererID);
		indexBuffer->bind();

		m_IndexBuffer = indexBuffer;
	}

}