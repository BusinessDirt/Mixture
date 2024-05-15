#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Mixture/Scene/Components.h"
#include <cstring>

/* The Microsoft C++ compiler is non-compliant with the C++ standard and needs
 * the following definition to disable a security warning on std::strncpy().
 */
#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

namespace Mixture {
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context) {
		setContext(context);
	}

	void SceneHierarchyPanel::setContext(const Ref<Scene>& context) {
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::onImGuiRender() {
		ImGui::Begin("Scene Hierarchy");
		
		m_Context->m_Registry.each([&](auto entityID){
			Entity entity{ entityID, m_Context.get() };
			drawEntityNode(entity);
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = {};

		// right click on blank space
		if (ImGui::BeginPopupContextWindow(0)) {
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->createEntity("Empty Entity");

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext) {
			drawComponents(m_SelectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::drawEntityNode(Entity entity) {
		std::string& tag = entity.getComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened) {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened) ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted) {
			m_Context->destroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static void drawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void drawComponent(const std::string& name, Entity entity, UIFunction uiFunction) {
		if (!entity.hasComponent<T>()) return;

		T& component = entity.getComponent<T>();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();

		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) ImGui::OpenPopup("ComponentSettings");

		ImGui::PopStyleVar();

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings")) {
			if (ImGui::MenuItem("Remove Component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open) {
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent)
			entity.removeComponent<T>();
	}

	void SceneHierarchyPanel::drawComponents(Entity entity) {
		if (entity.hasComponent<TagComponent>()) {
			std::string& tag = entity.getComponent<TagComponent>().tag;
			
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent")) {
			if (ImGui::MenuItem("Camera")) {
				if (!m_SelectionContext.hasComponent<CameraComponent>())
					m_SelectionContext.addComponent<CameraComponent>();
				else
					MX_CORE_WARN("This entity already has the Camera Component!");
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprite Renderer")) {
				if (!m_SelectionContext.hasComponent<SpriteRendererComponent>())
					m_SelectionContext.addComponent<SpriteRendererComponent>();
				else
					MX_CORE_WARN("This entity already has the Sprite Renderer Component!");
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

		drawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component) {
			drawVec3Control("Translation", component.translation);

			glm::vec3 rotation = glm::degrees(component.rotation);
			drawVec3Control("Rotation", rotation);
			component.rotation = glm::radians(rotation);

			drawVec3Control("Scale", component.scale, 1.0f);
		});

		drawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component) {
			SceneCamera& camera = component.camera;

			ImGui::Checkbox("Primary", &component.primary);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.getProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {
				for (int i = 0; i < 2; i++) {
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.setProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (camera.getProjectionType() == SceneCamera::ProjectionType::Perspective) {
				float verticalFOV = glm::degrees(camera.getPerspectiveVerticalFOV());
				float perspectiveNear = camera.getPerspectiveNearClip();
				float perspectiveFar = camera.getPerspectiveFarClip();

				if (ImGui::DragFloat("Vertical FOV", &verticalFOV)) camera.setPerspectiveVerticalFOV(glm::radians(verticalFOV));
				if (ImGui::DragFloat("Near", &perspectiveNear)) camera.setPerspectiveNearClip(perspectiveNear);
				if (ImGui::DragFloat("Far", &perspectiveFar)) camera.setPerspectiveNearClip(perspectiveFar);
			}

			if (camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic) {
				float orthoSize = glm::degrees(camera.getOrthographicSize());
				float orthoNear = camera.getOrthographicNearClip();
				float orthoFar = camera.getOrthographicFarClip();

				if (ImGui::DragFloat("Vertical FOV", &orthoSize)) camera.setOrthographicSize(glm::radians(orthoSize));
				if (ImGui::DragFloat("Near", &orthoNear)) camera.setOrthographicNearClip(orthoNear);
				if (ImGui::DragFloat("Far", &orthoFar)) camera.setOrthographicFarClip(orthoFar);

				ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
			}
		});

		drawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component) {
			ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
		});
	}
}
