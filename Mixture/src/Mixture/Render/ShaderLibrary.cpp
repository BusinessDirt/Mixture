#include "mxpch.hpp"
#include "Mixture/Render/ShaderLibrary.hpp"
#include "Mixture/Assets/Shaders/ShaderAsset.hpp"
#include "Mixture/Render/PipelineCache.hpp"

namespace Mixture
{
    RHI::IGraphicsDevice* ShaderLibrary::s_Device = nullptr;
    AssetManager::ReloadCallbackHandle ShaderLibrary::s_ReloadCallbackHandle = 0;
    std::unordered_map<ShaderCacheKey, Ref<RHI::IShader>, ShaderCacheKeyHash> ShaderLibrary::s_Cache;
    std::unordered_map<UUID, uint64_t> ShaderLibrary::s_Versions;
    std::mutex ShaderLibrary::s_Mutex;

    void ShaderLibrary::Init(RHI::IGraphicsDevice& device)
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        if (s_Device == &device) return;

        s_Cache.clear();
        s_Device = &device;

        // Register reload callback
        if (s_ReloadCallbackHandle != 0)
        {
            AssetManager::Get().RemoveReloadCallback(s_ReloadCallbackHandle);
        }
        s_ReloadCallbackHandle = AssetManager::Get().AddReloadCallback([](AssetType type, UUID id)
        {
            if (type == AssetType::Shader)
            {
                // We use 0 for magic because Reload only needs the ID
                Reload(AssetHandle{ id, 0 });
            }
        });
    }

    void ShaderLibrary::Shutdown()
    {
        AssetManager::ReloadCallbackHandle callbackHandle = 0;
        {
            std::lock_guard<std::mutex> lock(s_Mutex);
            s_Cache.clear();
            s_Versions.clear();
            s_Device = nullptr;
            callbackHandle = s_ReloadCallbackHandle;
            s_ReloadCallbackHandle = 0;
        }

        if (callbackHandle != 0)
        {
            AssetManager::Get().RemoveReloadCallback(callbackHandle);
        }

        PipelineCache::Clear();
    }

    bool ShaderLibrary::IsInitialized()
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        return s_Device != nullptr;
    }

    RHI::IShader* ShaderLibrary::GetShader(AssetHandle handle, RHI::ShaderStage stage)
    {
        if (!handle) return nullptr;

        ShaderCacheKey key = { handle.ID, stage };
        uint64_t version = 0;
        RHI::IGraphicsDevice* device = nullptr;

        {
            std::lock_guard<std::mutex> lock(s_Mutex);
            version = s_Versions.try_emplace(handle.ID, 1).first->second;
            auto it = s_Cache.find(key);
            if (it != s_Cache.end())
                return it->second.get();

            device = s_Device;
            if (!device) return nullptr;
        }

        // Load and Create
        auto shaderAsset = AssetManager::Get().GetResource<ShaderAsset>(handle);
        if (shaderAsset && shaderAsset->IsValid())
        {
            const RHI::ShaderIdentity identity{ static_cast<uint64_t>(handle.ID), version, stage };
            auto shader = device->CreateShader(
                shaderAsset->GetBufferPointer(), shaderAsset->GetBufferSize(), stage, identity);

            RHI::IShader* result = nullptr;
            bool versionChanged = false;
            {
                std::lock_guard<std::mutex> lock(s_Mutex);
                versionChanged = s_Versions[handle.ID] != version;
                if (!versionChanged)
                {
                    auto it = s_Cache.try_emplace(key, shader).first;
                    result = it->second.get();
                }
            }

            // A reload raced this creation. Discard the stale object and resolve
            // the current version instead of reintroducing old code into the cache.
            if (versionChanged) return GetShader(handle, stage);
            return result;
        }

        OPAL_ERROR("AssetManager", "Failed to resolve ShaderAsset for UUID: {}", (uint64_t)handle.ID);
        return nullptr;
    }

    void ShaderLibrary::Reload(AssetHandle handle)
    {
        if (!handle.ID.IsValid()) return;

        {
            std::lock_guard<std::mutex> lock(s_Mutex);
            uint64_t& version = s_Versions[handle.ID];
            if (version == 0) version = 1;
            ++version;

            // Remove all stage variants for the reloaded asset.
            for (auto it = s_Cache.begin(); it != s_Cache.end(); )
            {
                if (it->first.AssetID == handle.ID)
                    it = s_Cache.erase(it);
                else
                    ++it;
            }
        }

        PipelineCache::InvalidateShader(static_cast<uint64_t>(handle.ID));

        OPAL_INFO("AssetManager", "Reloaded Shader Asset: {}", (uint64_t)handle.ID);
    }

    void ShaderLibrary::Clear()
    {
        {
            std::lock_guard<std::mutex> lock(s_Mutex);
            s_Cache.clear();
            s_Versions.clear();
        }

        PipelineCache::Clear();
    }
}
