#include "mxpch.hpp"
#include "Mixture/Assets/AssetRegistry.hpp"

namespace Mixture
{
    std::string AssetRegistry::NormalizePath(const std::filesystem::path& path)
    {
        std::string normalized = path.lexically_normal().generic_string();
#ifdef _WIN32
        std::transform(normalized.begin(), normalized.end(), normalized.begin(),
            [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
#endif
        return normalized;
    }

    void AssetRegistry::RegisterAsset(const AssetMetadata& metadata)
    {
        if (metadata.ID.IsValid() && metadata.Type < AssetType::Count)
        {
            std::unique_lock lock(m_Mutex);
            if (auto existing = m_Assets.find(metadata.ID); existing != m_Assets.end())
                m_PathIndex[static_cast<size_t>(existing->second.Type)].erase(NormalizePath(existing->second.FilePath));
            m_Assets[metadata.ID] = metadata;
            m_PathIndex[static_cast<size_t>(metadata.Type)][NormalizePath(metadata.FilePath)] = metadata.ID;
        }
        else
        {
            OPAL_WARN("AssetManager", "Attempted to register invalid asset metadata: {0}", metadata.FilePath.string());
        }
    }

    void AssetRegistry::UnregisterAsset(UUID id)
    {
        std::unique_lock lock(m_Mutex);
        if (auto it = m_Assets.find(id); it != m_Assets.end())
        {
            auto& index = m_PathIndex[static_cast<size_t>(it->second.Type)];
            const auto pathIt = index.find(NormalizePath(it->second.FilePath));
            if (pathIt != index.end() && pathIt->second == id) index.erase(pathIt);
            m_Assets.erase(it);
        }
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

    AssetMetadata AssetRegistry::FindByPath(AssetType type, const std::filesystem::path& path) const
    {
        if (type <= AssetType::None || type >= AssetType::Count) return {};

        std::shared_lock lock(m_Mutex);
        const auto& index = m_PathIndex[static_cast<size_t>(type)];
        const auto pathIt = index.find(NormalizePath(path));
        if (pathIt == index.end()) return {};
        const auto assetIt = m_Assets.find(pathIt->second);
        return assetIt != m_Assets.end() ? assetIt->second : AssetMetadata{};
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
        for (auto& index : m_PathIndex) index.clear();
        m_Redirectors.clear();
    }
}
