#pragma once

#include "Mixture/Render/Graph/RenderGraphDefinitions.hpp"

namespace Mixture
{
    class RenderGraph;

    class ImGuiPass : public RenderPass
    {
    public:
        ImGuiPass(RGResourceHandle outputHandle);
        ~ImGuiPass() = default;

        void Setup(RenderGraphBuilder& builder) override;
        void Execute(const RenderGraphRegistry& registry, RHI::ICommandList* cmdList) const override;

    private:
        RGResourceHandle m_OutputHandle;
        RGResourceHandle m_Output;
    };
}
