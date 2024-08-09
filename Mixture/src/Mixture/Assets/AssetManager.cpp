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
        
        m_ShaderManager = CreateScope<ShaderManager>(m_AssetsPath / "shaders");
    }

    AssetManager::~AssetManager()
    {
        m_ShaderManager = nullptr;
    }

    const ShaderCode& AssetManager::GetShader(const std::string& filename) const
    {
        return m_ShaderManager->m_Shaders.at(filename);
    }
}