#include "mxpch.h"
#include "Mixture/Renderer/RenderCommand.h"

namespace Mixture {

	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::create();

}