#include "mxpch.h"
#include "RenderCommand.h"

#include "Mixture/Platform/OpenGL/OpenGLRendererAPI.h"

namespace Mixture {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}