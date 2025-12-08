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
    

    // -------------------------------------------------------------------------
    // Resource Nodes (The "Data")
    // -------------------------------------------------------------------------
    struct RGTextureNode 
    {
        RGResourceHandle Handle;
        std::string Name;
        RHI::TextureDesc Desc;
        // bool IsImported = false; // Is this the Backbuffer?
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
        
        // The actual execution logic (Recorded lambda)
        // We pass the Registry so you can look up the REAL texture later
        std::function<void(RenderGraphRegistry&, RHI::ICommandList*)> Execute;
    };
}
