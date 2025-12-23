#pragma once
#include "Mixture/Core/Base.hpp"

#include "Mixture/Assets/IAsset.hpp"
#include "Mixture/Assets/IAssetLoader.hpp"

#include <array>

namespace Mixture
{
    class AssetManager
    {
    public:
        static AssetManager& Get() { static AssetManager instance; return instance; }

        void Init();

        template<typename T>
        AssetHandle<T> GetAsset(const std::filesystem::path& path)
        {
            // Create the UUID deterministically from the path (Phase 1 Logic)
            UUID id = UUID::FromPath(path.string());

            // Check if already loaded
            if (m_Assets.find(id) != m_Assets.end())
            {
                return AssetHandle<T>{ id };
            }

            // If not loaded, load it now
            Ref<IAsset> newAsset = LoadAssetInternal(path, id);

            if (newAsset)
            {
                // Determine if the loaded type matches T (Safety check)
                // TODO: use dynamic_cast or assertion
                return AssetHandle<T>{ id };
            }

            return Handle<T>{ UUID(0) }; // Return Invalid Handle
        }

        // Retrieval: Used by systems (Renderer) to get the raw pointer
        template<typename T>
        T* GetResource(AssetHandle<T> handle)
        {
            if (!handle.ID.IsValid()) return nullptr;

            auto it = m_Assets.find(handle.ID);
            if (it != m_Assets.end()) return static_cast<T*>(it->second);
            return nullptr;
        }

    private:
        Ref<IAsset> LoadAssetInternal(const std::filesystem::path& path, UUID id);
        AssetType GetTypeFromExtension(const std::filesystem::path& path);

    private:
        std::unordered_map<UUID, Ref<IAsset>> m_Assets;
        std::unordered_map<AssetType, Scope<IAssetLoader>> m_Loaders;
    };
}
