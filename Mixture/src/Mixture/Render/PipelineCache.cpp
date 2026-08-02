#include "mxpch.hpp"
#include "Mixture/Render/PipelineCache.hpp"

namespace Mixture {

    RHI::IGraphicsDevice* PipelineCache::s_Device = nullptr;
    std::mutex PipelineCache::s_Mutex;
    std::unordered_map<RHI::PipelineDesc, Ref<RHI::IPipeline>, PipelineCache::PipelineDescHash> PipelineCache::s_Cache;

    void PipelineCache::Init(RHI::IGraphicsDevice& device)
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        if (s_Device == &device) return;

        s_Cache.clear();
        s_Device = &device;
    }

    void PipelineCache::Shutdown()
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        s_Cache.clear();
        s_Device = nullptr;
    }

    bool PipelineCache::IsInitialized()
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        return s_Device != nullptr;
    }

    RHI::IPipeline* PipelineCache::GetPipeline(const RHI::PipelineDesc& desc)
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        auto it = s_Cache.find(desc);
        if (it != s_Cache.end())
            return it->second.get();

        if (!s_Device)
        {
            OPAL_ERROR("Core/Render", "PipelineCache not initialized!");
            return nullptr;
        }

        Ref<RHI::IPipeline> pipeline = s_Device->CreatePipeline(desc);
        s_Cache[desc] = pipeline;
        return pipeline.get();
    }

    void PipelineCache::Clear()
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        s_Cache.clear();
    }
}
