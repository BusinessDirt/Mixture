#include "mxpch.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Mixture {

	static GLenum shaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type)
		{
			case ShaderDataType::Float:    return GL_FLOAT;
			case ShaderDataType::Float2:   return GL_FLOAT;
			case ShaderDataType::Float3:   return GL_FLOAT;
			case ShaderDataType::Float4:   return GL_FLOAT;
			case ShaderDataType::Mat3:     return GL_FLOAT;
			case ShaderDataType::Mat4:     return GL_FLOAT;
			case ShaderDataType::Int:      return GL_INT;
			case ShaderDataType::Int2:     return GL_INT;
			case ShaderDataType::Int3:     return GL_INT;
			case ShaderDataType::Int4:     return GL_INT;
			case ShaderDataType::Bool:     return GL_BOOL;
		}

		MX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		MX_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		MX_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::bind() const {
		MX_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::unbind() const {
		MX_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
		MX_PROFILE_FUNCTION();

		MX_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->bind();

		const BufferLayout& layout = vertexBuffer->getLayout();
		for (const auto& element : layout) {

			switch (element.type) {
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(m_VertexBufferIndex, element.getComponentCount(), shaderDataTypeToOpenGLBaseType(element.type),
						element.normalized ? GL_TRUE : GL_FALSE, layout.getStride(), (const void*)element.offset);
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool: 
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribIPointer(m_VertexBufferIndex, element.getComponentCount(), shaderDataTypeToOpenGLBaseType(element.type),
						layout.getStride(), (const void*)element.offset);
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.getComponentCount();
					for (uint8_t i = 0; i < count; i++) {
						glEnableVertexAttribArray(m_VertexBufferIndex);
						glVertexAttribPointer(m_VertexBufferIndex, count, shaderDataTypeToOpenGLBaseType(element.type),
							element.normalized ? GL_TRUE : GL_FALSE, layout.getStride(), (const void*)(element.offset + sizeof(float) * count * i));
						glVertexAttribDivisor(m_VertexBufferIndex, 1);
						m_VertexBufferIndex++;
					}
				}
				default:
					MX_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
		MX_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->bind();

		m_IndexBuffer = indexBuffer;
	}

}
