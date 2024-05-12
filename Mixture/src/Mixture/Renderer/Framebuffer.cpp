#include "mxpch.h"
#include "Framebuffer.h"

#include "Mixture/Renderer/Renderer.h"

#include "Mixture/Platform/OpenGL/OpenGLFramebuffer.h"

namespace Mixture {
    Ref<Framebuffer> Framebuffer::create(const FramebufferSpecification& spec) {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None: MX_CORE_ASSERT(false, "Renderer::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL: return createRef<OpenGLFramebuffer>(spec);
        }

        MX_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
