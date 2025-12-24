#include "mxpch.hpp"
#include "Mixture/Render/PipelineCache.hpp"

namespace Mixture {

    RHI::IGraphicsDevice* PipelineCache::s_Device = nullptr;
    std::unordered_map<RHI::PipelineDesc, Ref<RHI::IPipeline>, PipelineCache::PipelineDescHash> PipelineCache::s_Cache;

    void PipelineCache::Init(RHI::IGraphicsDevice& device)
    {
        s_Device = &device;
    }

    void PipelineCache::Shutdown()
    {
        s_Cache.clear();
        s_Device = nullptr;
    }

    RHI::IPipeline* PipelineCache::GetPipeline(const RHI::PipelineDesc& desc)
    {
        auto it = s_Cache.find(desc);
        if (it != s_Cache.end())
            return it->second;

        OPAL_ASSERT("Renderer", s_Device, "PipelineCache not initialized!");

        Ref<RHI::IPipeline> pipeline = s_Device->CreatePipeline(desc);
        s_Cache[desc] = pipeline;
        return pipeline;
    }

    void PipelineCache::Clear()
    {
        s_Cache.clear();
    }
}
