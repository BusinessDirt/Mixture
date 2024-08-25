#include "mxpch.hpp"
#include "AssetManager.hpp"

namespace Mixture
{
    namespace Util
    {
        std::filesystem::path GetProjectRootPath()
        {
            std::filesystem::path currentPath = std::filesystem::current_path();
            while (!currentPath.empty())
            {
                if (std::filesystem::exists(currentPath / ".git")) return currentPath;
                currentPath = currentPath.parent_path();
            }

            return {};
        }
    }

    AssetManager::AssetManager()
    {
        // TODO: replace "App" with a macro or similiar
        std::filesystem::path relativePath = Util::GetProjectRootPath() / "App" / "assets";
        m_AssetsPath = std::filesystem::absolute(relativePath);
    }

    std::filesystem::path AssetManager::GetTexturePath(const std::string& filename) const
    {
        return m_AssetsPath / "textures" / filename;
    }

    std::filesystem::path AssetManager::GetModelPath(const std::string& filename) const
    {
        return m_AssetsPath / "models" / filename;
    }

    std::filesystem::path AssetManager::GetShaderPath() const
    {
        return m_AssetsPath / "shaders";
    }
}
