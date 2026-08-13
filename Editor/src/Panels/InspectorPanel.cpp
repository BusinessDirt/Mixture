#include "Panels/InspectorPanel.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <typeinfo>

namespace Mixture
{
    InspectorPanel::InspectorPanel(std::shared_ptr<SceneHierarchyPanel> hierarchyPanel)
        : IEditorPanel("Inspector", true), m_HierarchyPanel(std::move(hierarchyPanel))
    {
    }

    void InspectorPanel::OnDrawImGui()
    {
        ImGui::Begin(m_Name.c_str(), &m_IsOpen);

        Entity selectedEntity = m_HierarchyPanel ? m_HierarchyPanel->GetSelectedEntity() : Entity{};

        if (selectedEntity.IsValid())
        {
            DrawComponents(selectedEntity);
        }
        else
        {
            ImGui::TextDisabled("No Entity Selected");
        }

        ImGui::End();
    }

    template<typename T, typename UIFunc>
    void InspectorPanel::DrawComponentUI(const std::string& name, Entity entity, UIFunc uiFunc)
    {
        if (!entity.HasComponent<T>()) return;

        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
            | ImGuiTreeNodeFlags_Framed
            | ImGuiTreeNodeFlags_SpanAvailWidth
            | ImGuiTreeNodeFlags_AllowOverlap
            | ImGuiTreeNodeFlags_FramePadding;

        auto& component = entity.GetComponent<T>();
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImGui::Separator();
        
        bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(T).hash_code()), treeNodeFlags, "%s", name.c_str());
        ImGui::PopStyleVar();

        bool removeComponent = false;
        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
        if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
        {
            ImGui::OpenPopup("ComponentSettings");
        }

        if (ImGui::BeginPopup("ComponentSettings"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                removeComponent = true;
            }
            ImGui::EndPopup();
        }

        if (open)
        {
            uiFunc(component);
            ImGui::TreePop();
        }

        if (removeComponent)
        {
            entity.RemoveComponent<T>();
        }
    }

    void InspectorPanel::DrawComponents(Entity entity)
    {
        // Tag / Active state Component UI
        if (entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>();

            ImGui::Checkbox("##Active", &tag.Active);
            ImGui::SameLine();

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy(buffer, tag.Name.c_str(), sizeof(buffer) - 1);
            if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
            {
                tag.Name = std::string(buffer);
            }
        }

        // Transform Component UI
        DrawComponentUI<TransformComponent>("Transform", entity, [](TransformComponent& transform) {
            DrawVec3Control("Position", transform.Position);
            glm::vec3 rotationDeg = glm::degrees(transform.GetRotationEuler());
            DrawVec3Control("Rotation", rotationDeg);
            transform.SetRotationEuler(glm::radians(rotationDeg));
            DrawVec3Control("Scale", transform.Scale, 1.0f);
        });

        // Mesh Renderer Component UI
        DrawComponentUI<MeshRendererComponent>("Mesh Renderer", entity, [](MeshRendererComponent& meshRenderer) {
            ImGui::Checkbox("Enabled", &meshRenderer.Enabled);

            char pathBuf[256];
            memset(pathBuf, 0, sizeof(pathBuf));
            strncpy(pathBuf, meshRenderer.MeshPath.c_str(), sizeof(pathBuf) - 1);
            if (ImGui::InputText("Mesh Path", pathBuf, sizeof(pathBuf)))
            {
                meshRenderer.MeshPath = std::string(pathBuf);
            }

            if (!meshRenderer.MaterialAsset)
            {
                if (ImGui::Button("Create Material"))
                {
                    meshRenderer.MaterialAsset = Material::Create("New Material");
                }
            }
            else
            {
                if (ImGui::TreeNodeEx("Material Properties", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    const auto& matData = static_cast<const Material&>(*meshRenderer.MaterialAsset).GetData();
                    glm::vec4 albedoColor = matData.AlbedoColor;
                    float roughness = matData.Roughness;
                    float metallic = matData.Metallic;
                    glm::vec3 emissionColor = matData.EmissionColor;
                    float emissionIntensity = matData.EmissionIntensity;
                    glm::vec2 tiling = matData.Tiling;

                    if (ImGui::ColorEdit4("Albedo Color", &albedoColor.x)) meshRenderer.MaterialAsset->SetAlbedoColor(albedoColor);
                    if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f)) meshRenderer.MaterialAsset->SetRoughness(roughness);
                    if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f)) meshRenderer.MaterialAsset->SetMetallic(metallic);
                    if (ImGui::ColorEdit3("Emission Color", &emissionColor.x)) meshRenderer.MaterialAsset->SetEmissionColor(emissionColor);
                    if (ImGui::DragFloat("Emission Intensity", &emissionIntensity, 0.1f, 0.0f, 100.0f)) meshRenderer.MaterialAsset->SetEmissionIntensity(emissionIntensity);
                    if (ImGui::DragFloat2("Tiling", &tiling.x, 0.1f)) meshRenderer.MaterialAsset->SetTiling(tiling);

                    ImGui::TreePop();
                }
            }
        });

        // Light Component UI
        DrawComponentUI<LightComponent>("Light Component", entity, [](LightComponent& light) {
            ImGui::ColorEdit3("Light Color", &light.Color.x);
            ImGui::DragFloat("Intensity", &light.Intensity, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat("Range", &light.Range, 0.1f, 0.0f, 1000.0f);

            const char* lightTypeNames[] = { "Directional", "Point", "Spot" };
            int currentType = static_cast<int>(light.Type);
            if (ImGui::Combo("Type", &currentType, lightTypeNames, 3))
            {
                light.Type = static_cast<LightType>(currentType);
            }

            if (light.Type == LightType::Spot)
            {
                ImGui::SliderFloat("Spot Angle", &light.SpotAngle, 1.0f, 179.0f);
            }

            ImGui::Checkbox("Enabled", &light.Enabled);
        });

        // Camera Component UI
        DrawComponentUI<CameraComponent>("Camera Component", entity, [](CameraComponent& camera) {
            ImGui::SliderFloat("Field Of View", &camera.Fov, 10.0f, 120.0f);
            ImGui::DragFloat("Near Clip", &camera.NearClip, 0.01f, 0.001f, 10.0f);
            ImGui::DragFloat("Far Clip", &camera.FarClip, 10.0f, 10.0f, 10000.0f);
            ImGui::Checkbox("Primary Camera", &camera.Primary);
            ImGui::Checkbox("Fixed Aspect Ratio", &camera.FixedAspectRatio);
            if (camera.FixedAspectRatio)
            {
                ImGui::DragFloat("Aspect Ratio", &camera.AspectRatio, 0.01f, 0.1f, 10.0f);
            }
        });

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Add Component Button & Popup
        if (ImGui::Button("Add Component", ImVec2(-1, 0)))
        {
            ImGui::OpenPopup("AddComponentPopup");
        }

        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            if (!entity.HasComponent<MeshRendererComponent>() && ImGui::MenuItem("Mesh Renderer"))
            {
                entity.AddComponent<MeshRendererComponent>();
                ImGui::CloseCurrentPopup();
            }
            if (!entity.HasComponent<LightComponent>() && ImGui::MenuItem("Light Component"))
            {
                entity.AddComponent<LightComponent>();
                ImGui::CloseCurrentPopup();
            }
            if (!entity.HasComponent<CameraComponent>() && ImGui::MenuItem("Camera Component"))
            {
                entity.AddComponent<CameraComponent>();
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
