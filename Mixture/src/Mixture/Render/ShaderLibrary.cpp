#include "mxpch.hpp"
#include "Mixture/Render/ShaderLibrary.hpp"
#include "Mixture/Assets/Shaders/ShaderAsset.hpp"

namespace Mixture
{
    RHI::IGraphicsDevice* ShaderLibrary::s_Device = nullptr;
    std::unordered_map<ShaderCacheKey, Ref<RHI::IShader>, ShaderCacheKeyHash> ShaderLibrary::s_Cache;
    std::mutex ShaderLibrary::s_Mutex;

    void ShaderLibrary::Init(RHI::IGraphicsDevice& device)
    {
        s_Device = &device;

        // Register reload callback
        AssetManager::Get().AddReloadCallback([](AssetType type, UUID id)
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
        Clear();
        s_Device = nullptr;
    }

    RHI::IShader* ShaderLibrary::GetShader(AssetHandle handle, RHI::ShaderStage stage)
    {
        if (!handle) return nullptr;

        ShaderCacheKey key = { handle.ID, stage };

        {
            std::lock_guard<std::mutex> lock(s_Mutex);
            auto it = s_Cache.find(key);
            if (it != s_Cache.end())
                return it->second.get();
        }

        // Load and Create
        auto shaderAsset = AssetManager::Get().GetResource<ShaderAsset>(handle);
        if (shaderAsset && shaderAsset->IsValid())
        {
            auto shader = s_Device->CreateShader(shaderAsset->GetBufferPointer(), shaderAsset->GetBufferSize(), stage);
            
            {
                std::lock_guard<std::mutex> lock(s_Mutex);
                s_Cache[key] = shader;
            }
            return shader.get();
        }

        OPAL_ERROR("AssetManager", "Failed to resolve ShaderAsset for UUID: {}", (uint64_t)handle.ID);
        return nullptr;
    }

    void ShaderLibrary::Reload(AssetHandle handle)
    {
        if (!handle.ID.IsValid()) return;

        std::lock_guard<std::mutex> lock(s_Mutex);

        // Iterate through the cache and remove ALL entries matching this Asset ID
        for (auto it = s_Cache.begin(); it != s_Cache.end(); )
        {
            if (it->first.AssetID == handle.ID)
            {
                // Erase returns the iterator to the next element
                it = s_Cache.erase(it);
            }
            else
            {
                ++it;
            }
        }

        OPAL_INFO("AssetManager", "Reloaded Shader Asset: {}", (uint64_t)handle.ID);
    }

    void ShaderLibrary::Clear()
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        s_Cache.clear();
    }
}
