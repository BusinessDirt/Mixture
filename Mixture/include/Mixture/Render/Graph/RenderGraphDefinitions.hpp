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
     * @brief Represents a texture resource node in the render graph (The "Data").
     */
    struct RGTextureNode 
    {
        RGResourceHandle Handle;
        std::string Name;
        RHI::TextureDesc Desc;
        
        /**
         * @brief Flag to prevent the graph from trying to free this memory.
         * For imports, we might hold the pointer directly here temporarily.
         */
        bool IsImported = false; 
        std::shared_ptr<RHI::ITexture> ExternalTexture = nullptr;

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
        Vector<RGResourceHandle> Writes;
        Vector<RGBarrier> Barriers;
        
        /**
         * @brief The actual execution logic (Recorded lambda).
         * We pass the Registry so you can look up the REAL texture later.
         */
        std::function<void(RenderGraphRegistry&, Ref<RHI::ICommandList>)> Execute;
    };
}
