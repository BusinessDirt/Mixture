#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraphRegistry.hpp"

namespace Mixture
{

    void RenderGraphRegistry::RegisterTexture(RGResourceHandle handle, RHI::ITexture* texture)
    {
        if (!handle.IsValid()) return;

        // Ensure vector is big enough
        if (handle.ID >= m_Textures.size())
        {
            m_Textures.resize(handle.ID + 1);
        }

        m_Textures[handle.ID] = texture;
    }

    RHI::ITexture* RenderGraphRegistry::GetTexture(RGResourceHandle handle)
    {
        if (!handle.IsValid()) {
            return nullptr;
        }

        // Safety check
        if (handle.ID >= m_Textures.size()) {
            // In a real engine, log "Error: Accessing unallocated texture handle!"
            return nullptr;
        }

        return m_Textures[handle.ID];
    }

    void RenderGraphRegistry::ImportTexture(RGResourceHandle handle, RHI::ITexture* texture)
    {
        // For now, importing is the same as registering.
        // Later, you might flag this resource as "Do Not Delete" or "External"
        RegisterTexture(handle, texture);
    }

    void RenderGraphRegistry::Clear()
    {
        // NOTE: We do NOT clear imported resources (like the Swapchain) if they persist,
        // but for a simple implementation, clearing the vector allows a fresh start next frame.
        // The shared_ptrs will keep the actual Vulkan objects alive if they are held elsewhere.
        m_Textures.clear();
        m_Buffers.clear();
    }
}
