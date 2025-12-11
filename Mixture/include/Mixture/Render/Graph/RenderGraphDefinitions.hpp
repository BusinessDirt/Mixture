#pragma once

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
    
    struct RGBarrier 
    {
        RGResourceHandle Resource;
        RHI::ResourceState Before;
        RHI::ResourceState After;
        // Flags like "Flush L2 Cache" or "Memory Access" can be added here
    };

    // -------------------------------------------------------------------------
    // Resource Nodes (The "Data")
    // -------------------------------------------------------------------------
    struct RGTextureNode 
    {
        RGResourceHandle Handle;
        std::string Name;
        RHI::TextureDesc Desc;
        
        // Flag to prevent the graph from trying to free this memory
        // For imports, we might hold the pointer directly here temporarily
        bool IsImported = false; 
        std::shared_ptr<RHI::ITexture> ExternalTexture = nullptr;
    };

    // -------------------------------------------------------------------------
    // Pass Nodes (The "Logic")
    // -------------------------------------------------------------------------
    struct RGPassNode 
    {
        std::string Name;
        
        // Dependencies (Built during Setup phase)
        Vector<RGResourceHandle> Reads;
        Vector<RGResourceHandle> Writes;

        std::vector<RGBarrier> Barriers;
        
        // The actual execution logic (Recorded lambda)
        // We pass the Registry so you can look up the REAL texture later
        std::function<void(RenderGraphRegistry&, RHI::ICommandList*)> Execute;
    };
}
