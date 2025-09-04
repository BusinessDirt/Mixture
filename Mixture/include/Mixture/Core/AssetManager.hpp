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
		OPAL_NODISCARD std::filesystem::path GetTexturePath() const;

	private:
		std::filesystem::path m_AssetsPath;
		Scope<Jasper::ShaderManager> m_ShaderManager;
	};
}