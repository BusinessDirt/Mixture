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
            m_Textures.resize(handle.ID + 1, nullptr);
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

    void RenderGraphRegistry::RegisterBuffer(RGResourceHandle handle, RHI::IBuffer* buffer)
    {
        if (!handle.IsValid()) return;

        // Ensure vector is big enough
        if (handle.ID >= m_Buffers.size())
        {
            m_Buffers.resize(handle.ID + 1, nullptr);
        }

        m_Buffers[handle.ID] = buffer;
    }

    RHI::IBuffer* RenderGraphRegistry::GetBuffer(RGResourceHandle handle)
    {
        if (!handle.IsValid()) {
            return nullptr;
        }

        if (handle.ID >= m_Buffers.size()) {
            return nullptr;
        }

        return m_Buffers[handle.ID];
    }

    void RenderGraphRegistry::ImportTexture(RGResourceHandle handle, RHI::ITexture* texture)
    {
        RegisterTexture(handle, texture);
    }

    void RenderGraphRegistry::ImportBuffer(RGResourceHandle handle, RHI::IBuffer* buffer)
    {
        RegisterBuffer(handle, buffer);
    }

    void RenderGraphRegistry::Clear()
    {
        m_Textures.clear();
        m_Buffers.clear();
    }
}