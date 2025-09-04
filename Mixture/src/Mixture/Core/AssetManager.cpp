#include "mxpch.hpp"
#include "Mixture/Core/AssetManager.hpp"

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

		Jasper::ShaderManager::Settings shaderManagerSettings{};
		shaderManagerSettings.Debug = true;
		shaderManagerSettings.Environment = Jasper::ShaderCompiler::TargetEnvironment::Vulkan;
		shaderManagerSettings.AssetDirectory = m_AssetsPath;
		
		m_ShaderManager = CreateScope<Jasper::ShaderManager>(shaderManagerSettings);
	}

	const Jasper::SPVShader& AssetManager::GetShader(const char* shaderName) const
	{
		return m_ShaderManager->GetShader(shaderName);
	}

	std::filesystem::path AssetManager::GetTexturePath() const
	{
		return m_AssetsPath / "textures";
	}
}
