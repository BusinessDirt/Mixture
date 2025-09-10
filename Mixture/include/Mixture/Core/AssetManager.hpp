#pragma once

#include "Mixture/Core/Base.hpp"

#include "Jasper.hpp"

namespace Mixture
{
	class AssetManager
	{
	public:
		OPAL_NON_COPIABLE(AssetManager);
		
		AssetManager();
		~AssetManager() = default;

		OPAL_NODISCARD const Jasper::SPVShader& GetShader(const char* shaderName) const;

		template<typename T>
		OPAL_NODISCARD Jasper::TextureHandle LoadTexture(const char* textureName,
			const bool global = true) const
		{
			return m_TextureManager->Load<T>(textureName, global);
		}

		template<typename T>
		OPAL_NODISCARD Jasper::TextureHandle CreateTexture(const Jasper::TextureSpecification& specification,
			const bool global = true) const
		{
			return m_TextureManager->Create<T>(specification, global);
		}

		OPAL_NODISCARD std::weak_ptr<Jasper::Texture> GetTexture(Jasper::TextureHandle textureHandle) const;
		void UnloadTexture(Jasper::TextureHandle textureHandle) const;
		void UnloadSceneTextures() const;
		void UnloadGlobalTextures() const;
		void UnloadAllTextures() const;

	private:
		std::filesystem::path m_AssetsPath;
		
		Scope<Jasper::ShaderManager> m_ShaderManager;
		Scope<Jasper::TextureManager> m_TextureManager;
	};
}
