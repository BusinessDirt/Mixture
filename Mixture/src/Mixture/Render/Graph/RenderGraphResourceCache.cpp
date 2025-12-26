#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraphResourceCache.hpp"

namespace Mixture
{
    RenderGraphResourceCache::RenderGraphResourceCache(RHI::IGraphicsDevice& device)
        : m_Device(device)
    {
    }

    RenderGraphResourceCache::~RenderGraphResourceCache()
    {
        Clear();
    }

    Ref<RHI::ITexture> RenderGraphResourceCache::GetOrCreateTexture(const std::string& name, const RHI::TextureDesc& desc)
    {
        TextureKey key{ name, desc };
        auto it = m_TextureCache.find(key);
        if (it != m_TextureCache.end())
        {
            return it->second;
        }

        // Create and cache
        auto texture = m_Device.CreateTexture(desc);
        m_TextureCache[key] = texture;
        return texture;
    }

    Ref<RHI::IBuffer> RenderGraphResourceCache::GetOrCreateBuffer(const std::string& name, const RHI::BufferDesc& desc)
    {
        BufferKey key{ name, desc };
        auto it = m_BufferCache.find(key);
        if (it != m_BufferCache.end())
        {
            return it->second;
        }

        // Create and cache
        auto buffer = m_Device.CreateBuffer(desc);
        m_BufferCache[key] = buffer;
        return buffer;
    }

    void RenderGraphResourceCache::Clear()
    {
        m_TextureCache.clear();
        m_BufferCache.clear();
    }
}
