#pragma once

#include "Mixture/Render/RHI/IPipeline.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"
#include <unordered_map>

namespace Mixture {

    class PipelineCache
    {
    public:
        static void Init(RHI::IGraphicsDevice& device);
        static void Shutdown();

        static Ref<RHI::IPipeline> GetPipeline(const RHI::PipelineDesc& desc);
        static void Clear();

    private:
        struct PipelineDescHash
        {
            std::size_t operator()(const RHI::PipelineDesc& desc) const
            {
                std::size_t h = 0;
                auto hashCombine = [](std::size_t& seed, std::size_t val) {
                    seed ^= val + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                };

                hashCombine(h, std::hash<void*>{}(desc.VertexShader));
                hashCombine(h, std::hash<void*>{}(desc.FragmentShader));

                // Rasterizer
                hashCombine(h, (size_t)desc.Rasterizer.polygonMode);
                hashCombine(h, (size_t)desc.Rasterizer.cullMode);
                hashCombine(h, (size_t)desc.Rasterizer.frontFace);
                // Hash float as int representation for stability? Or just cast.
                // Assuming standard IEEE 754, bit_cast or reinterpret_cast is okay for hashing equality.
                // But let's just use the value if it's typical 1.0f. 
                // A safer way for floats in hash:
                // hashCombine(h, std::hash<float>{}(desc.Rasterizer.lineWidth)); 

                // DepthStencil
                hashCombine(h, desc.DepthStencil.depthTest);
                hashCombine(h, desc.DepthStencil.depthWrite);
                hashCombine(h, (size_t)desc.DepthStencil.depthCompareOp);

                // Blend
                hashCombine(h, desc.Blend.enabled);
                hashCombine(h, (size_t)desc.Blend.srcColor);
                hashCombine(h, (size_t)desc.Blend.dstColor);
                hashCombine(h, (size_t)desc.Blend.colorOp);
                hashCombine(h, (size_t)desc.Blend.srcAlpha);
                hashCombine(h, (size_t)desc.Blend.dstAlpha);
                hashCombine(h, (size_t)desc.Blend.alphaOp);

                // Topology
                hashCombine(h, (size_t)desc.Topology);

                // Formats
                for (auto f : desc.ColorAttachmentFormats)
                    hashCombine(h, (size_t)f);
                
                hashCombine(h, (size_t)desc.DepthAttachmentFormat);

                return h;
            }
        };

        static RHI::IGraphicsDevice* s_Device;
        static std::unordered_map<RHI::PipelineDesc, Ref<RHI::IPipeline>, PipelineDescHash> s_Cache;
    };
}
