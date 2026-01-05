#include "mxpch.hpp"
#include "Mixture/Render/ImGui/ImGuiPass.hpp"

#include "Mixture/Render/Graph/RenderGraph.hpp"
#include "Mixture/Render/RHI/IGraphicsContext.hpp"
#include "Mixture/Core/Application.hpp"

namespace Mixture
{
    void ImGuiPass::AddToGraph(RenderGraph& graph, RGResourceHandle outputHandle)
    {
        graph.AddPass<ImGuiPassData>("ImGuiPass",
            [=](RenderGraphBuilder& builder, ImGuiPassData& data)
            {
                RGAttachmentInfo info;
                info.Handle = outputHandle;
                info.LoadOp = RHI::LoadOp::Load;
                info.StoreOp = RHI::StoreOp::Store;

                data.Output = builder.Write(info);
            },
            [=](const RenderGraphRegistry& registry, const ImGuiPassData& data, RHI::ICommandList* cmd)
            {

            }
        );
    }
}
