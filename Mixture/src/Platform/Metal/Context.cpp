#if defined(OPAL_PLATFORM_DARWIN)
#include "Platform/Metal/Context.hpp"

namespace Mixture::Metal
{

    static Context* s_Instance = nullptr;
    static const int MAX_FRAMES_IN_FLIGHT = 2;

    Context& Context::Get()
    {
        if (!s_Instance) throw std::logic_error("No Vulkan context exists");
        return *s_Instance;
    }

    Context::Context(const ApplicationDescription& appDescription, void* windowHandle)
    {
        s_Instance = this;
    }

    Context::~Context()
    {
        s_Instance = nullptr;
    }

    RHI::IGraphicsDevice& Context::GetDevice() const
    {

    }

    void Context::OnResize(uint32_t width, uint32_t height)
    {

    }

    RHI::ITexture* Context::BeginFrame()
    {

    }

    void Context::EndFrame()
    {

    }

    Scope<RHI::ICommandList> Context::GetCommandBuffer()
    {

    }

    uint32_t Context::GetSwapchainWidth() const
    {

    }

    uint32_t Context::GetSwapchainHeight() const
    {

    }
}

#endif
