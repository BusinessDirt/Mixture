#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/RHI/IGraphicsDevice.hpp"
#include "Mixture/Render/RHI/ResourceStates.hpp"

#include <vector>

namespace Mixture::RHI 
{
    /**
     * Helper struct for Dynamic Rendering (Vulkan 1.3).
     */
    struct RenderingAttachment 
    {
        /**
         * The texture to write to.
         */
        ITexture* Image = nullptr;

        /**
         * Clear or Load existing?
         */
        RHI::LoadOp LoadOp = LoadOp::Clear;

        /**
         * Store operation.
         */
        RHI::StoreOp StoreOp = StoreOp::Store;

        /**
         * Clear color.
         */
        float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

        /**
         * Depth clear value.
         */
        float DepthClearValue = 1.0f;
    };

    /**
     * Structure describing rendering information.
     */
    struct RenderingInfo 
    {
        /**
         * List of color attachments.
         */
        std::vector<RenderingAttachment> ColorAttachments;

        /**
         * Pointer to the depth attachment.
         */
        RenderingAttachment* DepthAttachment = nullptr;
        
        /**
         * Render Area X coordinate.
         */
        uint32_t RenderAreaX = 0;

        /**
         * Render Area Y coordinate.
         */
        uint32_t RenderAreaY = 0;

        /**
         * Render Area Width.
         */
        uint32_t RenderAreaWidth = 0;

        /**
         * Render Area Height.
         */
        uint32_t RenderAreaHeight = 0;
    };

    /**
     * Interface for a command list.
     */
    class ICommandList 
    {
    public:
        /**
         * Virtual destructor.
         */
        virtual ~ICommandList() = default;

        // ---------------------------------------------------------------------
        // Recording Control
        // ---------------------------------------------------------------------

        /**
         * Begins command list recording.
         */
        virtual void Begin() = 0;

        /**
         * Ends command list recording.
         */
        virtual void End() = 0;

        /**
         * Starts a Dynamic Rendering block (replaces VkRenderPass).
         * 
         * @param info The rendering information.
         */
        virtual void BeginRendering(const RenderingInfo& info) = 0;

        /**
         * Ends a Dynamic Rendering block.
         */
        virtual void EndRendering() = 0;

        // ---------------------------------------------------------------------
        // State Setup
        // ---------------------------------------------------------------------

        /**
         * Sets the viewport.
         * 
         * @param x The X coordinate.
         * @param y The Y coordinate.
         * @param width The width.
         * @param height The height.
         * @param minDepth The minimum depth.
         * @param maxDepth The maximum depth.
         */
        virtual void SetViewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f) = 0;

        /**
         * Sets the scissor rectangle.
         * 
         * @param x The X coordinate.
         * @param y The Y coordinate.
         * @param width The width.
         * @param height The height.
         */
        virtual void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height) = 0;

        // ---------------------------------------------------------------------
        // Binding
        // ---------------------------------------------------------------------

        /**
         * Binds a pipeline.
         * 
         * @param pipeline The pipeline to bind.
         */
        virtual void BindPipeline(IPipeline* pipeline) = 0;

        /**
         * Binds a vertex buffer.
         * 
         * @param buffer The buffer to bind.
         * @param binding The binding index.
         */
        virtual void BindVertexBuffer(IBuffer* buffer, uint32_t binding = 0) = 0;

        /**
         * Binds an index buffer.
         * 
         * @param buffer The buffer to bind.
         */
        virtual void BindIndexBuffer(IBuffer* buffer) = 0;

        /**
         * Creates a pipeline image barrier for the specified texture.
         * 
         * @param texture the texture to create the barrier for
         * @param oldState the old state of the layout
         * @param newState the new state of the layout
         */
        virtual void PipelineBarrier(ITexture* texture, ResourceState oldState, ResourceState newState) = 0;

        // ---------------------------------------------------------------------
        // Push Constants (Fast, small data upload)
        // ---------------------------------------------------------------------

        /**
         * Updates push constants.
         * 
         * @param pipeline The pipeline layout compatible with the push constants.
         * @param stage The shader stage flags.
         * @param data Pointer to the data.
         * @param size Size of the data in bytes.
         */
        virtual void PushConstants(IPipeline* pipeline, ShaderStage stage, const void* data, uint32_t size) = 0;

        // ---------------------------------------------------------------------
        // Drawing
        // ---------------------------------------------------------------------

        /**
         * Draws primitives.
         * 
         * @param vertexCount The number of vertices to draw.
         * @param instanceCount The number of instances to draw.
         * @param firstVertex The index of the first vertex.
         * @param firstInstance The instance ID of the first instance.
         */
        virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;

        /**
         * Draws indexed primitives.
         * 
         * @param indexCount The number of indices to draw.
         * @param instanceCount The number of instances to draw.
         * @param firstIndex The base index within the index buffer.
         * @param vertexOffset The value added to the vertex index before indexing into the vertex buffer.
         * @param firstInstance The instance ID of the first instance.
         */
        virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;
    };
}
