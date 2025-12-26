#pragma once

/**
 * @file RenderGraphDefinitions.hpp
 * @brief Definitions for RenderGraph nodes, resources, and barriers.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/Graph/RenderGraphHandle.hpp"
#include "Mixture/Render/RHI/RHI.hpp"

#include <string>
#include <vector>
#include <functional>

namespace Mixture
{

    namespace RHI
    {
        class ICommandList; // Forward Declaration
    }

    class RenderGraphRegistry; // Forward Declaration

    /**
     * @brief Represents a resource barrier to transition resource states between passes.
     */
    struct RGBarrier
    {
        RGResourceHandle Resource;
        RHI::ResourceState Before;
        RHI::ResourceState After;
        // Flags like "Flush L2 Cache" or "Memory Access" can be added here
    };

    /**
     * @brief Defines how a resource is written to (Attachment Info).
     */
    struct RGAttachmentInfo
    {
        RGResourceHandle Handle;
        RHI::LoadOp LoadOp = RHI::LoadOp::Clear;
        RHI::StoreOp StoreOp = RHI::StoreOp::Store;
        float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float DepthClearValue = 1.0f;
    };

    enum class RGResourceType : uint8_t
    {
        Texture,
        Buffer,
        ImportedTexture,
        ImportedBuffer
    };

    /**
     * @brief Represents a resource node in the render graph (The "Data").
     * Can be a Texture or a Buffer.
     */
    struct RGResourceNode
    {
        RGResourceHandle Handle;
        std::string Name;
        RGResourceType Type;

        // Descriptors (Only one is valid based on Type)
        RHI::TextureDesc TextureDesc;
        RHI::BufferDesc BufferDesc;

        // External Pointers (For imports)
        RHI::ITexture* ExternalTexture = nullptr;
        RHI::IBuffer* ExternalBuffer = nullptr;

        // --- Lifetime Metadata ---

        /** @brief Index of the first pass that uses this resource. Initialize to -1 to indicate "Not Used". */
        int32_t FirstPassIndex = -1;
        /** @brief Index of the last pass that uses this resource. */
        int32_t LastPassIndex = -1;
    };

    /**
     * @brief Represents a pass node in the render graph (The "Logic").
     */
    struct RGPassNode
    {
        std::string Name;

        // Dependencies (Built during Setup phase)
        Vector<RGResourceHandle> Reads;
        Vector<RGAttachmentInfo> Writes; // Texture Attachments
        Vector<RGResourceHandle> BufferWrites; // Buffer/Storage writes

        Vector<RGBarrier> Barriers;

        /**
         * @brief The actual execution logic (Recorded lambda).
         * We pass the Registry so you can look up the REAL texture later.
         */
        std::function<void(RenderGraphRegistry&, RHI::ICommandList*)> Execute;
    };
}
