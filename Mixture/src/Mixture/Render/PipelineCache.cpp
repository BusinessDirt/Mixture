#include "mxpch.hpp"
#include "Mixture/Render/PipelineCache.hpp"

namespace Mixture {

    RHI::IGraphicsDevice* PipelineCache::s_Device = nullptr;
    std::mutex PipelineCache::s_Mutex;
    std::unordered_map<PipelineCache::PipelineKey, Ref<RHI::IPipeline>, PipelineCache::PipelineKeyHash> PipelineCache::s_Cache;

    PipelineCache::PipelineKey PipelineCache::MakeKey(const RHI::PipelineDesc& desc)
    {
        PipelineKey key;
        if (desc.VertexShader) key.VertexShader = desc.VertexShader->GetIdentity();
        if (desc.FragmentShader) key.FragmentShader = desc.FragmentShader->GetIdentity();
        key.Rasterizer = desc.Rasterizer;
        key.DepthStencil = desc.DepthStencil;
        key.Blend = desc.Blend;
        key.Topology = desc.Topology;
        key.ColorAttachmentFormats = desc.ColorAttachmentFormats;
        key.DepthAttachmentFormat = desc.DepthAttachmentFormat;
        return key;
    }

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
        const PipelineKey key = MakeKey(desc);
        if (!key.VertexShader || (desc.FragmentShader && !key.FragmentShader))
        {
            OPAL_ERROR("Core/Render", "Pipeline shaders require stable identities before caching!");
            return nullptr;
        }

        std::lock_guard<std::mutex> lock(s_Mutex);
        auto it = s_Cache.find(key);
        if (it != s_Cache.end())
            return it->second.get();

        if (!s_Device)
        {
            OPAL_ERROR("Core/Render", "PipelineCache not initialized!");
            return nullptr;
        }

        Ref<RHI::IPipeline> pipeline = s_Device->CreatePipeline(desc);
        if (!pipeline) return nullptr;

        s_Cache[key] = pipeline;
        return pipeline.get();
    }

    void PipelineCache::InvalidateShader(uint64_t stableShaderID)
    {
        if (stableShaderID == 0) return;

        std::lock_guard<std::mutex> lock(s_Mutex);
        for (auto it = s_Cache.begin(); it != s_Cache.end(); )
        {
            if (it->first.VertexShader.StableID == stableShaderID
                || it->first.FragmentShader.StableID == stableShaderID)
            {
                it = s_Cache.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void PipelineCache::Clear()
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        s_Cache.clear();
    }
}
