#include "mxpch.hpp"
#include "Mixture/Render/ImGui/ImGuiPass.hpp"

#include "Mixture/Render/Graph/RenderGraphBuilder.hpp"
#include "Mixture/Render/ImGui/Context.hpp"

namespace Mixture
{
    void ImGuiPass::Setup(RenderGraphBuilder& builder)
    {
        RGAttachmentInfo output;
        output.Handle = m_OutputHandle;

        if (m_InputTextureHandle.IsValid())
        {
            builder.Read(m_InputTextureHandle);
            output.LoadOp = RHI::LoadOp::Clear;
            output.ClearColor[0] = 0.1f;
            output.ClearColor[1] = 0.1f;
            output.ClearColor[2] = 0.1f;
            output.ClearColor[3] = 1.0f;
        }
        else
        {
            output.LoadOp = RHI::LoadOp::Load;
        }

        output.StoreOp = RHI::StoreOp::Store;
        builder.Write(output);
    }

    void ImGuiPass::Execute(const RenderGraphRegistry&, RHI::ICommandList* commandList) const
    {
        m_Context->Render(commandList);
    }
}
