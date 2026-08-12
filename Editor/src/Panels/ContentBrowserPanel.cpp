#include "Panels/ContentBrowserPanel.hpp"

#include <imgui.h>

namespace Mixture
{
    ContentBrowserPanel::ContentBrowserPanel(std::filesystem::path assetsPath)
        : IEditorPanel("Content Browser", true)
        , m_BaseDirectory(std::move(assetsPath))
        , m_CurrentDirectory(m_BaseDirectory)
    {
        if (!std::filesystem::exists(m_BaseDirectory))
        {
            std::filesystem::create_directories(m_BaseDirectory);
        }
    }

    void ContentBrowserPanel::OnDrawImGui()
    {
        ImGui::Begin(m_Name.c_str(), &m_IsOpen);

        // Top Navigation Bar
        if (m_CurrentDirectory != m_BaseDirectory)
        {
            if (ImGui::Button("<- Back"))
            {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
            ImGui::SameLine();
        }

        std::string relPath = std::filesystem::relative(m_CurrentDirectory, m_BaseDirectory).string();
        ImGui::Text("Assets/%s", relPath == "." ? "" : relPath.c_str());

        ImGui::Separator();

        float cellSize = m_ThumbnailSize + m_Padding;
        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = static_cast<int>(panelWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        ImGui::Columns(columnCount, nullptr, false);

        if (std::filesystem::exists(m_CurrentDirectory))
        {
            for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
            {
                const auto& path = directoryEntry.path();
                std::string filenameString = path.filename().string();

                ImGui::PushID(filenameString.c_str());
                
                bool isDirectory = directoryEntry.is_directory();
                const char* icon = isDirectory ? "[Dir]" : "[File]";
                
                if (!isDirectory)
                {
                    std::string ext = path.extension().string();
                    if (ext == ".png" || ext == ".jpg" || ext == ".dds") icon = "[Tex]";
                    else if (ext == ".slang" || ext == ".spv" || ext == ".glsl") icon = "[Shd]";
                    else if (ext == ".gltf" || ext == ".obj" || ext == ".fbx") icon = "[Mesh]";
                }

                ImGui::Button(icon, ImVec2(m_ThumbnailSize, m_ThumbnailSize));

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (isDirectory)
                    {
                        m_CurrentDirectory /= path.filename();
                    }
                }

                ImGui::TextWrapped("%s", filenameString.c_str());

                ImGui::NextColumn();
                ImGui::PopID();
            }
        }

        ImGui::Columns(1);

        ImGui::End();
    }
}
