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

	extern const std::filesystem::path g_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context) {
		setContext(context);
	}

	void SceneHierarchyPanel::setContext(const Ref<Scene>& context) {
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::onImGuiRender() {
		ImGui::Begin("Scene Hierarchy");
		
		if (m_Context) {
			m_Context->m_Registry.each([&](auto entityID) {
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
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext) {
			drawComponents(m_SelectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::setSelectedEntity(Entity entity) {
		m_SelectionContext = entity;
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

	template<typename T>
	static void drawAddComponent(const std::string& name, Entity& entity) {
		if (!entity.hasComponent<T>()) {
			if (ImGui::MenuItem(name.c_str())) {
				entity.addComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
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
			drawAddComponent<CameraComponent>("Camera", m_SelectionContext);
			drawAddComponent<SpriteRendererComponent>("Sprite Renderer", m_SelectionContext);
			drawAddComponent<CircleRendererComponent>("Circle Renderer", m_SelectionContext);
			drawAddComponent<Rigidbody2DComponent>("Rigidbody 2D", m_SelectionContext);
			drawAddComponent<BoxCollider2DComponent>("Box Collider 2D", m_SelectionContext);
			drawAddComponent<CircleCollider2DComponent>("Circle Collider 2D", m_SelectionContext);

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

			ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					Ref<Texture2D> texture = Texture2D::create(texturePath.string());
					if (texture->isLoaded())
						component.texture = texture;
					else
						MX_WARN("Could not load texture {0}", texturePath.filename().string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("Tiling Factor", &component.tilingFactor, 0.1f, 0.0f, 100.0f);
		});

		drawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component) {
			ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
			ImGui::DragFloat("Thickness", &component.thickness, 0.025f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &component.fade, 0.00025f, 0.0f, 1.0f);
		});

		drawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](Rigidbody2DComponent& component){
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.type];
			if (ImGui::BeginCombo("Body Type", currentBodyTypeString)) {
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected)) {
						currentBodyTypeString = bodyTypeStrings[i];
						component.type = (Rigidbody2DComponent::BodyType)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.fixedRotation);
		});

		drawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](BoxCollider2DComponent& component) {
			ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.size));
			ImGui::DragFloat("Density", &component.density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.restitutionThreshold, 0.01f, 0.0f);
		});

		drawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](CircleCollider2DComponent& component) {
			ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
			ImGui::DragFloat("Radius", &component.radius);
			ImGui::DragFloat("Density", &component.density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.restitutionThreshold, 0.01f, 0.0f);
		});
	}
}
