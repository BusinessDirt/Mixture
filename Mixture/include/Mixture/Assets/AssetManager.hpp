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
        void SetAssetRoot(const std::filesystem::path& rootPath);

        AssetHandle GetAsset(AssetType type, const std::filesystem::path& path);

        // Retrieval: Used by systems (Renderer) to get the pointer
        template<typename T>
        Ref<T> GetResource(AssetHandle handle)
        {
            if (!handle.ID.IsValid()) return nullptr;

            auto it = m_Assets.find(handle.ID);
            if (it != m_Assets.end())
            {
                return std::static_pointer_cast<T>(it->second);
            }
            
            return nullptr;
        }

    private:
        Ref<IAsset> LoadAssetInternal(AssetType type, const std::filesystem::path& path, UUID id);

    private:
        std::filesystem::path m_RootDirectory;
        std::unordered_map<UUID, Ref<IAsset>> m_Assets;
        std::unordered_map<AssetType, Scope<IAssetLoader>> m_Loaders;
    };
}
