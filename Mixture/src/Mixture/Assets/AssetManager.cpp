#include "mxpch.hpp"
#include "Mixture/Assets/AssetManager.hpp"

namespace Mixture
{
	namespace Util
	{
		namespace 
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
	}

	AssetManager::AssetManager()
	{
		const std::filesystem::path relativePath = Util::GetProjectRootPath() / "App" / "assets";
		m_AssetsPath = std::filesystem::absolute(relativePath);
	}

	std::filesystem::path AssetManager::GetShaderPath() const
	{
		return m_AssetsPath / "shaders";
	}

	std::filesystem::path AssetManager::GetTexturePath() const
	{
		return m_AssetsPath / "textures";
	}
}
