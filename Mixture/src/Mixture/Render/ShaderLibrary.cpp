#include "mxpch.hpp"
#include "Mixture/Render/ShaderLibrary.hpp"
#include "Mixture/Assets/Shaders/ShaderAsset.hpp"

namespace Mixture
{
    RHI::IGraphicsDevice* ShaderLibrary::s_Device = nullptr;
    std::unordered_map<UUID, Ref<RHI::IShader>> ShaderLibrary::s_Cache;

    void ShaderLibrary::Init(RHI::IGraphicsDevice& device)
    {
        s_Device = &device;
    }

    void ShaderLibrary::Shutdown()
    {
        Clear();
        s_Device = nullptr;
    }

    RHI::IShader* ShaderLibrary::GetShader(AssetHandle handle, RHI::ShaderStage stage)
    {
        if (!handle.ID.IsValid()) return nullptr;

        auto it = s_Cache.find(handle.ID);
        if (it != s_Cache.end())
        {
            return it->second.get();
        }

        // Load and Create
        auto shaderAsset = AssetManager::Get().GetResource<ShaderAsset>(handle);
        if (shaderAsset && shaderAsset->IsValid())
        {
            auto shader = s_Device->CreateShader(shaderAsset->GetBufferPointer(), shaderAsset->GetBufferSize(), stage);
            s_Cache[handle.ID] = shader;
            return shader.get();
        }

        OPAL_ERROR("Core/Assets", "Failed to resolve ShaderAsset for UUID: {}", (uint64_t)handle.ID);
        return nullptr;
    }

    void ShaderLibrary::Reload(AssetHandle handle)
    {
        if (!handle.ID.IsValid()) return;

        // Removing from cache will force GetShader to recreate it next time it's requested
        // assuming the AssetManager has already reloaded the ShaderAsset data.
        s_Cache.erase(handle.ID);
    }

    void ShaderLibrary::Clear()
    {
        s_Cache.clear();
    }
}
