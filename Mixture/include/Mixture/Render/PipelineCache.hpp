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

        /**
         * @brief Clears the internal cache.
         */
        static void Clear();

    private:
        struct PipelineDescHash
        {
            std::size_t operator()(const RHI::PipelineDesc& desc) const
            {
                std::size_t seed = 0;

                Util::HashCombine(seed, desc.VertexShader, desc.FragmentShader);

                // Rasterizer
                Util::HashCombine(seed, desc.Rasterizer.polygonMode,
                                        desc.Rasterizer.cullMode,
                                        desc.Rasterizer.frontFace,
                                        desc.Rasterizer.lineWidth);

                // DepthStencil
                Util::HashCombine(seed, desc.DepthStencil.depthTest,
                                        desc.DepthStencil.depthWrite,
                                        desc.DepthStencil.depthCompareOp);

                // Blend
                Util::HashCombine(seed, desc.Blend.enabled,
                                        desc.Blend.srcColor,
                                        desc.Blend.dstColor,
                                        desc.Blend.colorOp,
                                        desc.Blend.srcAlpha,
                                        desc.Blend.dstAlpha,
                                        desc.Blend.alphaOp);

                // Topology
                Util::HashCombine(seed, desc.Topology);

                // Formats
                for (auto f : desc.ColorAttachmentFormats) Util::HashCombine(seed, f);
                Util::HashCombine(seed, desc.DepthAttachmentFormat);

                return seed;
            }
        };

        static RHI::IGraphicsDevice* s_Device;
        static std::mutex s_Mutex;
        static std::unordered_map<RHI::PipelineDesc, Ref<RHI::IPipeline>, PipelineDescHash> s_Cache;
    };
}
