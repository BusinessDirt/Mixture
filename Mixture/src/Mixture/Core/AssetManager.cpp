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

		Jasper::ShaderManager::Settings shaderManagerSettings;
		shaderManagerSettings.Debug = true;
		shaderManagerSettings.Environment = Jasper::ShaderCompiler::TargetEnvironment::Vulkan;
		shaderManagerSettings.AssetDirectory = m_AssetsPath;
		shaderManagerSettings.ShaderDirectoryName = "shaders";
		shaderManagerSettings.ShaderCacheDirectoryName = "cache";
		m_ShaderManager = CreateScope<Jasper::ShaderManager>(shaderManagerSettings);

		Jasper::TextureManager::Settings textureManagerSettings;
		textureManagerSettings.AssetDirectory = m_AssetsPath;
		textureManagerSettings.TextureDirectoryName = "textures";
		m_TextureManager = CreateScope<Jasper::TextureManager>(textureManagerSettings);
	}

	const Jasper::SPVShader& AssetManager::GetShader(const char* shaderName) const
	{
		return m_ShaderManager->Get(shaderName);
	}

	std::weak_ptr<Jasper::Texture> AssetManager::GetTexture(const Jasper::TextureHandle textureHandle) const
	{
		return m_TextureManager->Get(textureHandle);
	}

	void AssetManager::UnloadTexture(const Jasper::TextureHandle textureHandle) const
	{
		m_TextureManager->Unload(textureHandle);
	}

	void AssetManager::UnloadSceneTextures() const
	{
		m_TextureManager->UnloadScene();
	}

	void AssetManager::UnloadGlobalTextures() const
	{
		m_TextureManager->UnloadGlobal();
	}

	void AssetManager::UnloadAllTextures() const
	{
		m_TextureManager->UnloadAll();
	}
}
