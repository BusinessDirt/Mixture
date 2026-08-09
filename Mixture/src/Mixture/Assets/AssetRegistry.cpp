#include "mxpch.hpp"
#include "Mixture/Assets/AssetRegistry.hpp"

namespace Mixture
{
    void AssetRegistry::RegisterAsset(const AssetMetadata& metadata)
    {
        if (metadata.ID.IsValid())
        {
            std::unique_lock lock(m_Mutex);
            m_Assets[metadata.ID] = metadata;
        }
        else
        {
            OPAL_WARN("AssetManager", "Attempted to register asset with invalid UUID: {0}", metadata.FilePath.string());
        }
    }

    void AssetRegistry::UnregisterAsset(UUID id)
    {
        std::unique_lock lock(m_Mutex);
        m_Assets.erase(id);
    }

    bool AssetRegistry::Contains(UUID id) const
    {
        std::shared_lock lock(m_Mutex);
        return m_Assets.find(id) != m_Assets.end();
    }

    AssetMetadata AssetRegistry::GetMetadata(UUID id) const
    {
        std::shared_lock lock(m_Mutex);
        auto it = m_Assets.find(id);
        if (it != m_Assets.end())
        {
            return it->second;
        }
        return {};
    }

    std::filesystem::path AssetRegistry::GetPath(UUID id) const
    {
        std::shared_lock lock(m_Mutex);
        auto it = m_Assets.find(id);
        if (it != m_Assets.end())
        {
            return it->second.FilePath;
        }
        return {};
    }

    std::unordered_map<UUID, AssetMetadata> AssetRegistry::GetAssets() const
    {
        std::shared_lock lock(m_Mutex);
        return m_Assets;
    }

    void AssetRegistry::AddRedirector(AssetType type, const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
    {
        std::unique_lock lock(m_Mutex);
        // Store using generic_string to ensure consistency across platforms
        m_Redirectors[type][oldPath.generic_string()] = newPath.generic_string();
    }

    std::filesystem::path AssetRegistry::ResolvePath(AssetType type, const std::filesystem::path& path) const
    {
        std::shared_lock lock(m_Mutex);
        auto typeIt = m_Redirectors.find(type);
        if (typeIt == m_Redirectors.end())
        {
            return path;
        }

        const auto& redirectMap = typeIt->second;
        std::string currentPathStr = path.generic_string();
        bool redirected = false;

        // Follow redirection chain (max depth 10 to prevent infinite loops)
        for (int i = 0; i < 10; ++i)
        {
            auto it = redirectMap.find(currentPathStr);
            if (it == redirectMap.end())
            {
                if (redirected)
                {
                    OPAL_LOG_DEBUG("AssetManager", "Asset path redirected: '{}' -> '{}'", path.string(), currentPathStr);
                }

                return std::filesystem::path(currentPathStr);
            }
            // Found a redirect, update current path and continue
            currentPathStr = it->second;
            redirected = true;
        }

        OPAL_ERROR("AssetManager", "Redirector loop detected for asset path: {}", path.string());
        return std::filesystem::path(currentPathStr);
    }

    void AssetRegistry::Clear()
    {
        std::unique_lock lock(m_Mutex);
        m_Assets.clear();
        m_Redirectors.clear();
    }
}
