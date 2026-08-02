#pragma once

/**
 * @file PipelineCache.hpp
 * @brief Cache for Pipeline State Objects (PSOs).
 */

#include "Mixture/Render/RHI/IPipeline.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"
#include "Mixture/Util/Util.hpp"
#include <unordered_map>

namespace Mixture {

    /**
     * @brief Caches created pipelines to avoid redundant state creation.
     */
    class PipelineCache
    {
    public:
        /**
         * @brief Initializes the pipeline cache.
         * 
         * @param device The graphics device used for creating pipelines.
         */
        static void Init(RHI::IGraphicsDevice& device);

        /**
         * @brief Shuts down the pipeline cache and clears all cached pipelines.
         */
        static void Shutdown();

        /** @brief Returns whether the cache is bound to a graphics device. */
        static bool IsInitialized();

        /**
         * @brief Retrieves a pipeline from the cache, or creates it if it doesn't exist.
         * 
         * @param desc The description of the pipeline to get/create.
         * @return RHI::IPipeline* Pointer to the pipeline.
         */
        static RHI::IPipeline* GetPipeline(const RHI::PipelineDesc& desc);

        /** @brief Releases pipelines that depend on the given logical shader. */
        static void InvalidateShader(uint64_t stableShaderID);

        /**
         * @brief Clears the internal cache.
         */
        static void Clear();

    private:
        struct PipelineKey
        {
            RHI::ShaderIdentity VertexShader;
            RHI::ShaderIdentity FragmentShader;
            RHI::RasterizerState Rasterizer;
            RHI::DepthStencilState DepthStencil;
            RHI::BlendState Blend;
            RHI::PrimitiveTopology Topology = RHI::PrimitiveTopology::TriangleList;
            Vector<RHI::Format> ColorAttachmentFormats;
            RHI::Format DepthAttachmentFormat = RHI::Format::Undefined;

            bool operator==(const PipelineKey&) const = default;
        };

        struct PipelineKeyHash
        {
            std::size_t operator()(const PipelineKey& key) const
            {
                std::size_t seed = 0;

                Util::HashCombine(seed,
                    key.VertexShader.StableID, key.VertexShader.Version, key.VertexShader.Stage,
                    key.FragmentShader.StableID, key.FragmentShader.Version, key.FragmentShader.Stage);

                // Rasterizer
                Util::HashCombine(seed, key.Rasterizer.polygonMode,
                                        key.Rasterizer.cullMode,
                                        key.Rasterizer.frontFace,
                                        key.Rasterizer.lineWidth);

                // DepthStencil
                Util::HashCombine(seed, key.DepthStencil.depthTest,
                                        key.DepthStencil.depthWrite,
                                        key.DepthStencil.depthCompareOp);

                // Blend
                Util::HashCombine(seed, key.Blend.enabled,
                                        key.Blend.srcColor,
                                        key.Blend.dstColor,
                                        key.Blend.colorOp,
                                        key.Blend.srcAlpha,
                                        key.Blend.dstAlpha,
                                        key.Blend.alphaOp);

                // Topology
                Util::HashCombine(seed, key.Topology);

                // Formats
                for (auto format : key.ColorAttachmentFormats) Util::HashCombine(seed, format);
                Util::HashCombine(seed, key.DepthAttachmentFormat);

                return seed;
            }
        };

        static PipelineKey MakeKey(const RHI::PipelineDesc& desc);

        static RHI::IGraphicsDevice* s_Device;
        static std::mutex s_Mutex;
        static std::unordered_map<PipelineKey, Ref<RHI::IPipeline>, PipelineKeyHash> s_Cache;
    };
}
