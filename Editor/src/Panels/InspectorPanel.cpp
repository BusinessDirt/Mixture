#include "Panels/InspectorPanel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace Mixture
{
    InspectorPanel::InspectorPanel(std::shared_ptr<SceneHierarchyPanel> hierarchyPanel)
        : IEditorPanel("Inspector", true), m_HierarchyPanel(std::move(hierarchyPanel))
    {
    }

    void InspectorPanel::OnDrawImGui()
    {
        ImGui::Begin(m_Name.c_str(), &m_IsOpen);

        EditorEntity* selectedEntity = m_HierarchyPanel ? m_HierarchyPanel->GetSelectedEntity() : nullptr;

        if (selectedEntity)
        {
            DrawComponents(*selectedEntity);
        }
        else
        {
            ImGui::TextDisabled("No Entity Selected");
        }

        ImGui::End();
    }

    void InspectorPanel::DrawComponents(EditorEntity& entity)
    {
        // Entity Active State & Tag/Name
        ImGui::Checkbox("##Active", &entity.Active);
        ImGui::SameLine();

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strncpy(buffer, entity.Name.c_str(), sizeof(buffer) - 1);
        if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
        {
            entity.Name = std::string(buffer);
        }

        ImGui::Separator();

        // Transform Component
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            DrawVec3Control("Position", entity.Position);
            glm::vec3 rotationDeg = glm::degrees(entity.Rotation);
            DrawVec3Control("Rotation", rotationDeg);
            entity.Rotation = glm::radians(rotationDeg);
            DrawVec3Control("Scale", entity.Scale, 1.0f);
        }

        // Mesh Renderer Component
        if (entity.HasMeshRenderer)
        {
            if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::ColorEdit4("Albedo Color", &entity.MaterialColor.x);
                ImGui::SliderFloat("Roughness", &entity.Roughness, 0.0f, 1.0f);
                ImGui::SliderFloat("Metallic", &entity.Metallic, 0.0f, 1.0f);
            }
        }

        // Light Component
        if (entity.HasLight)
        {
            if (ImGui::CollapsingHeader("Light Component", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::ColorEdit3("Light Color", &entity.LightColor.x);
                ImGui::DragFloat("Intensity", &entity.LightIntensity, 0.1f, 0.0f, 100.0f);
            }
        }

        // Camera Component
        if (entity.HasCamera)
        {
            if (ImGui::CollapsingHeader("Camera Component", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SliderFloat("Field Of View", &entity.Fov, 10.0f, 120.0f);
                ImGui::DragFloat("Near Clip", &entity.NearClip, 0.01f, 0.001f, 10.0f);
                ImGui::DragFloat("Far Clip", &entity.FarClip, 10.0f, 100.0f, 10000.0f);
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        
        // Add Component Popup
        if (ImGui::Button("Add Component", ImVec2(-1, 0)))
        {
            ImGui::OpenPopup("AddComponentPopup");
        }

        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            if (!entity.HasMeshRenderer && ImGui::MenuItem("Mesh Renderer"))
            {
                entity.HasMeshRenderer = true;
                ImGui::CloseCurrentPopup();
            }
            if (!entity.HasLight && ImGui::MenuItem("Light Component"))
            {
                entity.HasLight = true;
                ImGui::CloseCurrentPopup();
            }
            if (!entity.HasCamera && ImGui::MenuItem("Camera Component"))
            {
                entity.HasCamera = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void InspectorPanel::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });

        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        // X Axis
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Y Axis
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Z Axis
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();
    }
}
