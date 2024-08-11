#include "mxpch.hpp"
#include "DrawCommand.hpp"

#include "Mixture/Renderer/Renderer.hpp"

#include "Platform/Vulkan/DrawCommand.hpp"

namespace Mixture
{
    Scope<DrawCommand> DrawCommand::s_DrawCommand = DrawCommand::Create();

    Scope<DrawCommand> DrawCommand::Create()
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::Vulkan:  return CreateScope<Vulkan::DrawCommand>();
            case RendererAPI::API::OpenGL:  MX_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
            case RendererAPI::API::DirectX12:  MX_CORE_ASSERT(false, "RendererAPI::DirectX12 is currently not supported!"); return nullptr;
        }

        MX_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    void DrawCommand::Draw(CommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount,
        uint32_t firstVertex, uint32_t firstInstance)
    {
        s_DrawCommand->DrawCmd(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }
}
