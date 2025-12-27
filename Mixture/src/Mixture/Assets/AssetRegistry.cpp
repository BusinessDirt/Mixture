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
            OPAL_WARN("Core/Assets", "Attempted to register asset with invalid UUID: {0}", metadata.FilePath.string());
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

    void AssetRegistry::AddRedirector(AssetType type, const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
    {
        // Store using generic_string to ensure consistency across platforms
        m_Redirectors[type][oldPath.generic_string()] = newPath.generic_string();
    }

    std::filesystem::path AssetRegistry::ResolvePath(AssetType type, const std::filesystem::path& path) const
    {
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
                    OPAL_LOG_DEBUG("Core/Assets", "Asset path redirected: '{}' -> '{}'", path.string(), currentPathStr);
                }

                return std::filesystem::path(currentPathStr);
            }
            // Found a redirect, update current path and continue
            currentPathStr = it->second;
            redirected = true;
        }

        OPAL_ERROR("Core/Assets", "Redirector loop detected for asset path: {}", path.string());
        return std::filesystem::path(currentPathStr);
    }

    void AssetRegistry::Clear()
    {
        m_Assets.clear();
        m_Redirectors.clear();
    }
}
