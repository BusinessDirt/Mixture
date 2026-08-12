#pragma once

#include "Panels/IEditorPanel.hpp"
#include <filesystem>

namespace Mixture
{
    /**
     * @brief Panel responsible for browsing project asset files and directories.
     */
    class ContentBrowserPanel final : public IEditorPanel
    {
    public:
        explicit ContentBrowserPanel(std::filesystem::path assetsPath = "Assets");
        ~ContentBrowserPanel() override = default;

        void OnDrawImGui() override;

        /** Sets the root assets path for the browser. */
        void SetBaseDirectory(std::filesystem::path baseDirectory)
        {
            m_BaseDirectory = std::move(baseDirectory);
            m_CurrentDirectory = m_BaseDirectory;
        }

    private:
        std::filesystem::path m_BaseDirectory;
        std::filesystem::path m_CurrentDirectory;
        float m_Padding = 16.0f;
        float m_ThumbnailSize = 96.0f;
    };
}
