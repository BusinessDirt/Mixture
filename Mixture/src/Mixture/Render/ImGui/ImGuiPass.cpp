#include "mxpch.hpp"
#include "Mixture/Render/ImGui/ImGuiPass.hpp"

#include "Mixture/Render/Graph/RenderGraph.hpp"
#include "Mixture/Render/RHI/IGraphicsContext.hpp"
#include "Mixture/Core/Application.hpp"

namespace Mixture
{
    ImGuiPass::ImGuiPass(RGResourceHandle outputHandle)
        : m_OutputHandle(outputHandle)
    {
    }

    void ImGuiPass::Setup(RenderGraphBuilder& builder)
    {
        RGAttachmentInfo info;
        info.Handle = m_OutputHandle;
        info.LoadOp = RHI::LoadOp::Load;
        info.StoreOp = RHI::StoreOp::Store;

        m_Output = builder.Write(info);
    }

    void ImGuiPass::Execute(const RenderGraphRegistry& registry, RHI::ICommandList* cmdList) const
    {
        Application::Get().GetContext().RenderImGui(cmdList);
    }
}
