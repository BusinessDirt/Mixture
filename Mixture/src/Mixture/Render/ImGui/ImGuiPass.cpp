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
        output.LoadOp = RHI::LoadOp::Load;
        output.StoreOp = RHI::StoreOp::Store;
        builder.Write(output);
    }

    void ImGuiPass::Execute(const RenderGraphRegistry&, RHI::ICommandList* commandList) const
    {
        m_Context->Render(commandList);
    }
}
