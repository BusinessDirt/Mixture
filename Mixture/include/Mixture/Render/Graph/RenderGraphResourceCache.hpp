#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/RHI.hpp"
#include "Mixture/Util/Util.hpp"
#include <unordered_map>

namespace Mixture
{
    /**
     * @brief Caches render graph resources (Textures, Buffers) across frames.
     * Uses Name + Description as the key to ensure persistence for specific resources.
     */
    class RenderGraphResourceCache
    {
    public:
        RenderGraphResourceCache(RHI::IGraphicsDevice& device);
        ~RenderGraphResourceCache();

        Ref<RHI::ITexture> GetOrCreateTexture(const std::string& name, const RHI::TextureDesc& desc);
        Ref<RHI::IBuffer> GetOrCreateBuffer(const std::string& name, const RHI::BufferDesc& desc);

        /**
         * @brief Clears the cache, releasing all held resources.
         */
        void Clear();

    private:
        RHI::IGraphicsDevice& m_Device;

        struct TextureKey {
            std::string Name;
            RHI::TextureDesc Desc;
            bool operator==(const TextureKey& other) const { return Name == other.Name && Desc == other.Desc; }
        };
        struct TextureKeyHash {
            std::size_t operator()(const TextureKey& key) const {
                size_t seed = 0;
                Util::HashCombine(seed, key.Name, key.Desc.Width, key.Desc.Height, key.Desc.Format);
                return seed;
            }
        };

        struct BufferKey {
            std::string Name;
            RHI::BufferDesc Desc;
            bool operator==(const BufferKey& other) const { return Name == other.Name && Desc == other.Desc; }
        };
        struct BufferKeyHash {
            std::size_t operator()(const BufferKey& key) const {
                size_t seed = 0;
                Util::HashCombine(seed, key.Name, key.Desc.Size, key.Desc.Usage);
                return seed;
            }
        };

        std::unordered_map<TextureKey, Ref<RHI::ITexture>, TextureKeyHash> m_TextureCache;
        std::unordered_map<BufferKey, Ref<RHI::IBuffer>, BufferKeyHash> m_BufferCache;
    };
}
