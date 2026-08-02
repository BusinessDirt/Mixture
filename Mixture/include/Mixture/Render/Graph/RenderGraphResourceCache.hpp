#pragma once

/**
 * @file RenderGraphResourceCache.hpp
 * @brief Caching mechanism for RenderGraph transient resources.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/RHI.hpp"
#include "Mixture/Util/Util.hpp"
#include <unordered_map>

namespace Mixture
{
    /**
     * @brief Caches render graph resources (Textures, Buffers) across frames.
     * Pools resources by descriptor and synchronized frame slot. Logical resource
     * names do not create permanent cache entries.
     */
    class RenderGraphResourceCache
    {
    public:
        /**
         * @brief Constructs a RenderGraphResourceCache.
         *
         * @param device Reference to the graphics device.
         */
        RenderGraphResourceCache(RHI::IGraphicsDevice& device);
        ~RenderGraphResourceCache();

        /**
         * @brief Starts allocation for a synchronized frame slot.
         *
         * Entries unused for two completed uses of the same slot are retired.
         */
        void BeginFrame(uint32_t frameIndex);

        /** @brief Retrieves a pooled texture or creates one if necessary. */
        Ref<RHI::ITexture> GetOrCreateTexture(const RHI::TextureDesc& desc);

        /**
         * @brief Retrieves a cached buffer or creates a new one if not found.
         *
         * @param desc The description of the buffer.
         * @return Ref<RHI::IBuffer> The buffer.
         */
        Ref<RHI::IBuffer> GetOrCreateBuffer(const RHI::BufferDesc& desc);

        /**
         * @brief Clears the cache, releasing all held resources.
         */
        void Clear();

    private:
        RHI::IGraphicsDevice& m_Device;

        struct TextureKey {
            uint32_t Width;
            uint32_t Height;
            RHI::Format PixelFormat;
            RHI::ResourceState InitialState;
            bool operator==(const TextureKey&) const = default;
        };
        struct TextureKeyHash {
            std::size_t operator()(const TextureKey& key) const {
                size_t seed = 0;
                Util::HashCombine(seed, key.Width, key.Height, key.PixelFormat, key.InitialState);
                return seed;
            }
        };

        struct BufferKey {
            uint64_t Size;
            RHI::BufferUsage Usage;
            bool operator==(const BufferKey&) const = default;
        };
        struct BufferKeyHash {
            std::size_t operator()(const BufferKey& key) const {
                size_t seed = 0;
                Util::HashCombine(seed, key.Size, key.Usage);
                return seed;
            }
        };

        template<typename Resource>
        struct CacheEntry
        {
            Ref<Resource> ResourceRef;
            uint32_t FrameIndex = 0;
            bool UsedInLastFrame = true;
        };

        uint32_t m_CurrentFrameIndex = 0;
        bool m_FrameActive = false;

        std::unordered_map<TextureKey, Vector<CacheEntry<RHI::ITexture>>, TextureKeyHash> m_TextureCache;
        std::unordered_map<BufferKey, Vector<CacheEntry<RHI::IBuffer>>, BufferKeyHash> m_BufferCache;
    };
}
