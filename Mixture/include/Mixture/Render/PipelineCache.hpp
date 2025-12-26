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
        static void Init(RHI::IGraphicsDevice& device);
        static void Shutdown();

        static RHI::IPipeline* GetPipeline(const RHI::PipelineDesc& desc);
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
        static std::unordered_map<RHI::PipelineDesc, Ref<RHI::IPipeline>, PipelineDescHash> s_Cache;
    };
}
