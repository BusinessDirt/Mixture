#pragma once

#include "Mixture/Render/Graph/RenderGraphDefinitions.hpp"

namespace Mixture
{
    class ImGuiContext;

    /** Final compositing pass for application-owned Dear ImGui draw data. */
    class ImGuiPass final : public RenderPass
    {
    public:
        ImGuiPass(ImGuiContext& context, RGResourceHandle outputHandle, RGResourceHandle inputTextureHandle = {})
            : m_Context(&context), m_OutputHandle(outputHandle), m_InputTextureHandle(inputTextureHandle) {}

        void Setup(RenderGraphBuilder& builder) override;
        void Execute(const RenderGraphRegistry& registry, RHI::ICommandList* commandList) const override;

    private:
        ImGuiContext* m_Context;
        RGResourceHandle m_OutputHandle;
        RGResourceHandle m_InputTextureHandle;
    };
}
