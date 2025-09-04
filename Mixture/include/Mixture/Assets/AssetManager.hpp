#pragma once

#include "Mixture/Core/Base.hpp"

namespace Mixture
{
	class AssetManager
	{
	public:
		OPAL_NON_COPIABLE(AssetManager);
		
		AssetManager();
		~AssetManager() = default;

		OPAL_NODISCARD std::filesystem::path GetShaderPath() const;
		OPAL_NODISCARD std::filesystem::path GetTexturePath() const;

	private:
		std::filesystem::path m_AssetsPath;
	};
}