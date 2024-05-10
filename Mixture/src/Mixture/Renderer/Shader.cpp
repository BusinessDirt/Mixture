#include "mxpch.h"
#include "Mixture/Renderer/Shader.h"

#include "Mixture/Renderer/Renderer.h"
#include "Mixture/Platform/OpenGL/OpenGLShader.h"

namespace Mixture {

	Ref<Shader> Shader::create(const std::string& filepath) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None: MX_CORE_ASSERT(false, "RendererAPI:None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLShader>(filepath);
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None: MX_CORE_ASSERT(false, "RendererAPI:None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader) {
		MX_CORE_ASSERT(!exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::add(const Ref<Shader>& shader) {
		std::string name = shader->getName();
		add(name, shader);
	}

	Mixture::Ref<Mixture::Shader> ShaderLibrary::load(const std::string& filepath) {
		Ref<Shader> shader = Shader::create(filepath);
		add(shader);
		return shader;
	}

	Mixture::Ref<Mixture::Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath) {
		Ref<Shader> shader = Shader::create(filepath);
		add(name, shader);
		return shader;
	}

	Mixture::Ref<Mixture::Shader> ShaderLibrary::get(const std::string& name) {
		MX_CORE_ASSERT(exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::exists(const std::string& name) const {
		return m_Shaders.find(name) != m_Shaders.end();
	}
}