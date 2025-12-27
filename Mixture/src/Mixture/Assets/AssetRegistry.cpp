#include "mxpch.hpp"
#include "Mixture/Assets/AssetRegistry.hpp"

namespace Mixture
{
    void AssetRegistry::RegisterAsset(const AssetMetadata& metadata)
    {
        if (metadata.ID.IsValid())
        {
            m_Assets[metadata.ID] = metadata;
        }
        else
        {
            OPAL_WARN("AssetRegistry", "Attempted to register asset with invalid UUID: {0}", metadata.FilePath.string());
        }
    }

    void AssetRegistry::UnregisterAsset(UUID id)
    {
        m_Assets.erase(id);
    }

    bool AssetRegistry::Contains(UUID id) const
    {
        return m_Assets.find(id) != m_Assets.end();
    }

    const AssetMetadata& AssetRegistry::GetMetadata(UUID id) const
    {
        static AssetMetadata s_NullMetadata;
        auto it = m_Assets.find(id);
        if (it != m_Assets.end())
        {
            return it->second;
        }
        return s_NullMetadata;
    }

    const std::filesystem::path& AssetRegistry::GetPath(UUID id) const
    {
        static std::filesystem::path s_NullPath;
        auto it = m_Assets.find(id);
        if (it != m_Assets.end())
        {
            return it->second.FilePath;
        }
        return s_NullPath;
    }

    void AssetRegistry::Clear()
    {
        m_Assets.clear();
    }
}
