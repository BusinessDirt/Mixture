#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraphResourceCache.hpp"

#include <algorithm>

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

    void RenderGraphResourceCache::BeginFrame(uint32_t frameIndex)
    {
        m_CurrentFrameIndex = frameIndex;
        m_FrameActive = true;

        auto prepareCache = [frameIndex](auto& cache)
        {
            for (auto bucket = cache.begin(); bucket != cache.end();)
            {
                auto& entries = bucket->second;
                std::erase_if(entries, [frameIndex](const auto& entry)
                {
                    return entry.FrameIndex == frameIndex && !entry.UsedInLastFrame;
                });

                for (auto& entry : entries)
                {
                    if (entry.FrameIndex == frameIndex) entry.UsedInLastFrame = false;
                }

                if (entries.empty()) bucket = cache.erase(bucket);
                else ++bucket;
            }
        };

        prepareCache(m_TextureCache);
        prepareCache(m_BufferCache);
    }

    Ref<RHI::ITexture> RenderGraphResourceCache::GetOrCreateTexture(const RHI::TextureDesc& desc)
    {
        if (!m_FrameActive) return nullptr;

        TextureKey key{ desc.Width, desc.Height, desc.PixelFormat, desc.InitialState };
        auto& entries = m_TextureCache[key];
        for (auto& entry : entries)
        {
            if (entry.FrameIndex == m_CurrentFrameIndex && !entry.UsedInLastFrame)
            {
                entry.UsedInLastFrame = true;
                return entry.ResourceRef;
            }
        }

        auto texture = m_Device.CreateTexture(desc);
        if (texture)
        {
            entries.push_back({ texture, m_CurrentFrameIndex, true });
        }
        return texture;
    }

    Ref<RHI::IBuffer> RenderGraphResourceCache::GetOrCreateBuffer(const RHI::BufferDesc& desc)
    {
        if (!m_FrameActive) return nullptr;

        BufferKey key{ desc.Size, desc.Usage };
        auto& entries = m_BufferCache[key];
        for (auto& entry : entries)
        {
            if (entry.FrameIndex == m_CurrentFrameIndex && !entry.UsedInLastFrame)
            {
                entry.UsedInLastFrame = true;
                return entry.ResourceRef;
            }
        }

        auto buffer = m_Device.CreateBuffer(desc);
        if (buffer)
        {
            entries.push_back({ buffer, m_CurrentFrameIndex, true });
        }
        return buffer;
    }

    void RenderGraphResourceCache::Clear()
    {
        m_TextureCache.clear();
        m_BufferCache.clear();
        m_FrameActive = false;
    }
}
