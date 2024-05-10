#include "mxpch.h"
#include "RenderCommand.h"

#include "Mixture/Platform/OpenGL/OpenGLRendererAPI.h"

namespace Mixture {

	Scope<RendererAPI> RenderCommand::s_RendererAPI = createScope<OpenGLRendererAPI>();

}