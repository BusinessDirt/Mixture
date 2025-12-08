#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/Graph/RenderGraphHandle.hpp"
#include "Mixture/Render/RHI/ITexture.hpp"
#include "Mixture/Render/RHI/IBuffer.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <cassert>

namespace Mixture 
{

    class RenderGraphRegistry 
    {
    public:
        RenderGraphRegistry() = default;
        ~RenderGraphRegistry() = default;

        // ---------------------------------------------------------------------
        // Texture Management
        // ---------------------------------------------------------------------
        
        // Associate a handle with a real GPU resource
        void RegisterTexture(RGResourceHandle handle, Ref<RHI::ITexture> texture);

        // Retrieve the real GPU resource during pass execution
        // This is the function you call inside your Execute lambda!
        RHI::ITexture* GetTexture(RGResourceHandle handle);

        // ---------------------------------------------------------------------
        // External Resources (e.g. Backbuffer)
        // ---------------------------------------------------------------------
        
        // Import an existing texture (like the Swapchain image) into the graph
        // Returns a handle that passes can use to read/write to it.
        void ImportTexture(RGResourceHandle handle, Ref<RHI::ITexture> texture);

        // ---------------------------------------------------------------------
        // Maintenance
        // ---------------------------------------------------------------------
        void Clear();

    private:
        // We use a vector because handles are just indices (0, 1, 2...)
        // This is O(1) lookup speed.
        Vector<Ref<RHI::ITexture>> m_Textures;
        Vector<Ref<RHI::IBuffer>>  m_Buffers;
    };

}
