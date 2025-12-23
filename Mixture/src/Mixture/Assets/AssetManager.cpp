#include "mxpch.hpp"
#include "Mixture/Assets/AssetManager.hpp"

namespace Mixture
{
    void AssetManager::Init()
    {

    }

    Ref<IAsset> AssetManager::LoadAssetInternal(const std::filesystem::path& path, UUID id)
    {
        AssetType type = GetTypeFromExtension(path);
        if (type == AssetType::None)
        {
            OPAL_ERROR("Core/Assets", "Unknown file extension for file '{}'", path);
            return nullptr;
        }

        if (m_Loaders.find(type) == m_Loaders.end())
        {
            OPAL_ERROR("Core/Assets", "No loader registered for AssetType='{}'", Utils::AssetTypeToString(type));
            return nullptr;
        }

        AssetMetadata metadata;
        metadata.ID = id;
        metadata.Type = type;
        metadata.FilePath = path;

        FileStreamReader stream(path);
        if (!stream.IsOpen()) return nullptr;

        IAssetLoader& loader = *m_Loaders[type];
        Ref<IAsset> asset = loader.LoadSync(stream, metadata);

        if (asset) m_Assets[id] = asset;
        return asset;
    }

    AssetType AssetManager::GetTypeFromExtension(const std::filesystem::path& path)
    {
        auto ext = path.extension().string();
        if (ext == ".hlsl") return AssetType::Shader;
        if (ext == ".png" || ext == ".jpg") return AssetType::Texture;
        if (ext == ".obj" || ext == ".gltf") return AssetType::Mesh;
        return AssetType::None;
    }
}
