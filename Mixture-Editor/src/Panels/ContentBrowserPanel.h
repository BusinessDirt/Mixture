#pragma once

#include <filesystem>

#include "Mixture/Renderer/Texture.h"

namespace Mixture {
	class ContentBrowserPanel {
	public:
		ContentBrowserPanel();

		void onImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};
}
