#include <gtest/gtest.h>

#include "Mixture/Render/Graph/RenderGraph.hpp"
#include "Mixture/Render/RHI/ICommandList.hpp"
#include "Mixture/Render/RHI/ITexture.hpp"

using namespace Mixture;

// -----------------------------------------------------------------------------
// Mocks
// -----------------------------------------------------------------------------

// We need a dummy command list to pass to Execute()
namespace Mixture::RHI
{
    class MockCommandList : public ICommandList
    {
    public:
        // Implement all pure virtuals with empty bodies
        void Begin() override {}
        void End() override {}
        void BeginRendering(const RenderingInfo& info) override {}
        void EndRendering() override {}
        void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override {}
        void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height) override {}
        void BindPipeline(IPipeline* pipeline) override {}
        void PipelineBarrier(ITexture* texture, ResourceState oldState, ResourceState newState) override {}
        void BindVertexBuffer(IBuffer* buffer, uint32_t binding) override {}
        void BindIndexBuffer(IBuffer* buffer) override {}
        void PushConstants(IPipeline* pipeline, ShaderStage stage, const void* data, uint32_t size) override {}
        void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override {}
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override {}
    };
}



// -----------------------------------------------------------------------------
// The Test Case
// -----------------------------------------------------------------------------

TEST(RenderGraphTests, CanCompileAndExecutePass) {
    // 1. Setup
    RenderGraph graph;
    Mixture::RHI::MockCommandList cmd;
    bool lambdaWasExecuted = false;

    struct TestPassData {
        RGResourceHandle OutputTex;
    };

    // 2. Add a Pass
    graph.AddPass<TestPassData>("TestPass",
        // Setup Phase (Builder)
        [&](RenderGraphBuilder& builder, TestPassData& data) {
        Mixture::RHI::TextureDesc desc;
            desc.Width = 100;
            desc.Height = 100;
            desc.DebugName = "TestRenderTarget";

            // Verify we can create and write to a resource
            data.OutputTex = builder.Create("TestRT", desc);
            builder.Write(data.OutputTex);
            
            // Assertion inside the setup phase
            EXPECT_TRUE(data.OutputTex.IsValid());
        },
        // Execute Phase (Lambda)
        [&](RenderGraphRegistry& registry, TestPassData& data, RHI::ICommandList* cmdList) {
            // This code runs later when we call Execute()
            lambdaWasExecuted = true;
            
            // Verify we received the correct CommandList
            EXPECT_EQ(cmdList, &cmd);
            
            // Verify the handle survived
            EXPECT_TRUE(data.OutputTex.IsValid());
        }
    );

    // 3. Compile (Currently does nothing, but good to call)
    graph.Compile();

    // 4. Execute
    // This should trigger the second lambda above
    graph.Execute(&cmd);

    // 5. Final Assertion
    EXPECT_TRUE(lambdaWasExecuted) << "The pass execution lambda was never called!";
}
