#pragma once

#include "Mixture/Core/LayerStack.hpp"
#include "Mixture/Renderer/RendererAPI.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture
{
	class Renderer
	{
	public:
		static void Initialize(const std::string& applicationName);
		static void DestroyImGuiContext();
		static void Shutdown();

		static void OnFramebufferResize(const uint32_t width, const uint32_t height) { s_RendererAPI->SetViewport(0, 0, width, height); }
		static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }

		static void BeginFrame();
		static void EndFrame();
		
		static void BeginSceneRenderpass();
		static void EndSceneRenderpass();

		static void BeginImGuiImpl();
		static void RenderImGui();

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		static Scope<RendererAPI> s_RendererAPI;
		friend class RenderCommand;
	};
}
