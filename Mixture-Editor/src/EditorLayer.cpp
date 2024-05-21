#include "EditorLayer.h"
#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mixture/Scene/SceneSerializer.h"
#include "Mixture/Utils/PlatformUtils.h"

#include "ImGuizmo.h"
#include "Mixture/Math/Math.h"

namespace Mixture {

	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer() 
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f) {}
	
	void EditorLayer::onAttach() {
		MX_PROFILE_FUNCTION();

		FramebufferSpecification fbSpec;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.width = 1280;
		fbSpec.height = 720;
		m_Framebuffer = Framebuffer::create(fbSpec);

		m_IconPlay = Texture2D::create("resources/Icons/PlayButton.png");
		m_IconSimulate = Texture2D::create("resources/Icons/SimulateButton.png");
		m_IconStop = Texture2D::create("resources/Icons/StopButton.png");

		m_EditorScene = createRef<Scene>();
		m_ActiveScene = m_EditorScene;


		ApplicationCommandLineArgs commandLineArgs = Application::get().getCommandLineArgs();
		if (commandLineArgs.count > 1) {
			const char* sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.deserialize(sceneFilePath);
		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		m_SceneHierarchyPanel.setContext(m_ActiveScene);
	}

	void EditorLayer::onDetach() {
		MX_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Timestep ts) {
		MX_PROFILE_FUNCTION();

		// Resize
		if (Mixture::FramebufferSpecification spec = m_Framebuffer->getSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.width != m_ViewportSize.x || spec.height != m_ViewportSize.y)) {
			
			m_Framebuffer->resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.onResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.setViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Render
		Renderer2D::resetStats();
		m_Framebuffer->bind();
		RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::clear();

		// Clear out entity ID attachment to -1
		m_Framebuffer->clearAttachment(1, -1);

		switch (m_SceneState) {
		case SceneState::Edit:
			if (m_ViewportFocused)
				m_CameraController.onUpdate(ts);

			m_EditorCamera.onUpdate(ts);
			m_ActiveScene->onUpdateEditor(ts, m_EditorCamera);
			break;
		case SceneState::Simulate:
			m_EditorCamera.onUpdate(ts);
			m_ActiveScene->onUpdateSimulation(ts, m_EditorCamera);
			break;
		case SceneState::Play:
			m_ActiveScene->onUpdateRuntime(ts);
			break;
		
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
			int pixelData = m_Framebuffer->readPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}

		onOverlayRender();

		m_Framebuffer->unbind();
	}

	void EditorLayer::onImGuiRender() {
		MX_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("New", "Ctrl+N")) newScene();
				if (ImGui::MenuItem("Open...", "Ctrl+O")) openScene();
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) saveSceneAs();

				if (ImGui::MenuItem("Exit")) Mixture::Application::get().close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.onImGuiRender();
		m_ContentBrowserPanel.onImGuiRender();

		ImGui::Begin("Stats");

		std::string name = "None";
		if (m_HoveredEntity)
			name = m_HoveredEntity.getComponent<TagComponent>().tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = Mixture::Renderer2D::getStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		ImVec2 viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::get().getImGuiLayer()->blockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_Framebuffer->getColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
				const wchar_t* path = (const wchar_t*)payload->Data;
				openScene(std::filesystem::path(g_AssetPath) / path);
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.getSelectedEntity();
		if (selectedEntity && m_GizmoType != -1) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, 
				m_ViewportBounds[1].x - m_ViewportBounds[0].x, 
				m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Camera

			// Runtime camera from entity
			//Entity cameraEntity = m_ActiveScene->getPrimaryCameraEntity();
			//const SceneCamera& camera = cameraEntity.getComponent<CameraComponent>().camera;
			//const glm::mat4& cameraProjection = camera.getProjection();
			//glm::mat4 cameraView = glm::inverse(cameraEntity.getComponent<TransformComponent>().getTransform());

			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.getProjection();
			glm::mat4 cameraView = m_EditorCamera.getViewMatrix();

			// Entity transform
			TransformComponent& tc = selectedEntity.getComponent<TransformComponent>();
			glm::mat4 transform = tc.getTransform();

			// Snapping
			bool snap = Input::isKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				Math::decomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.rotation;
				tc.translation = translation;
				tc.rotation += deltaRotation;
				tc.scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		uiToolbar();

		ImGui::End();
		
	}

	void EditorLayer::uiToolbar() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = (bool)m_ActiveScene;
		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled) tintColor.w = 0.5f;

		float size = ImGui::GetWindowHeight() - 4.0f;

		{
			Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)icon->getRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, 
				ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled) {
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					onScenePlay();
				else if (m_SceneState == SceneState::Play)
					onSceneStop();
			}
		}
		ImGui::SameLine();
		{
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			//ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)icon->getRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0,
				ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled) {
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) {
					MX_CORE_INFO("test");
					onSceneSimulate();
				}
				else if (m_SceneState == SceneState::Simulate) {
					MX_CORE_INFO("test");
					onSceneStop();
				}
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::onEvent(Event& e) {
		m_CameraController.onEvent(e);
		if (m_SceneState == SceneState::Edit) m_EditorCamera.onEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<KeyPressedEvent>(MX_BIND_EVENT_FN(EditorLayer::onKeyPressed));
		dispatcher.dispatch<MouseButtonPressedEvent>(MX_BIND_EVENT_FN(EditorLayer::onMouseButtonPressed));
	}

	bool EditorLayer::onKeyPressed(KeyPressedEvent& e) {
		// Shortcuts
		if (e.isRepeat() > 0) return false;

		bool control = Input::isKeyPressed(Key::LeftControl) || Input::isKeyPressed(Key::RightControl);
		bool shift = Input::isKeyPressed(Key::LeftShift) || Input::isKeyPressed(Key::RightShift);
		
		switch (e.getKeyCode()) {
			case Key::N: 
				if (control) newScene();
				break;
			case Key::O: 
				if (control) openScene();
				break;
			case Key::S: 
				if (control) {
					if (shift) saveSceneAs();
					else saveScene();
				}
				break;

			// Scene Commands
			case Key::D:
				if (control)
					onDuplicateEntity();
				break;

			// Gizmos
			case Key::Q:
				if (!ImGuizmo::IsUsing()) m_GizmoType = -1;
				break;
			case Key::W:
				if (!ImGuizmo::IsUsing()) m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Key::E:
				if (!ImGuizmo::IsUsing()) m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case Key::R:
				if (!ImGuizmo::IsUsing()) m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
		}
	}

	bool EditorLayer::onMouseButtonPressed(MouseButtonPressedEvent& e) {
		if (e.getMouseButton() == Mouse::ButtonLeft) {
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::isKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.setSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::onOverlayRender() {
		if (m_SceneState == SceneState::Play) {
			Entity camera = m_ActiveScene->getPrimaryCameraEntity();
			if (!camera) return;
			Renderer2D::beginScene(camera.getComponent<CameraComponent>().camera, camera.getComponent<TransformComponent>().getTransform());
		} else Renderer2D::beginScene(m_EditorCamera);
		
		if (m_ShowPhysicsColliders) {
			// Box Colliders
			{
				auto view = m_ActiveScene->getAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (entt::entity entity : view) {
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 translation = tc.translation + glm::vec3(bc2d.offset, 0.001f);
					glm::vec3 scale = tc.scale * glm::vec3(bc2d.size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::rotate(glm::mat4(1.0f), tc.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::drawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}

			// Circle Colliders
			{
				auto view = m_ActiveScene->getAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (entt::entity entity : view) {
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.translation + glm::vec3(cc2d.offset, 0.001f);
					glm::vec3 scale = tc.scale * glm::vec3(cc2d.radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::drawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
				}
			}
		}

		// draw selected entity outline
		if (Entity selectedEntity = m_SceneHierarchyPanel.getSelectedEntity()) {
			TransformComponent transform = selectedEntity.getComponent<TransformComponent>();

			Renderer2D::drawRect(transform.getTransform(), glm::vec4(1, 0, 0, 1));
		}

		Renderer2D::endScene();
	}

	void EditorLayer::newScene() {
		m_ActiveScene = createRef<Scene>();
		m_ActiveScene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.setContext(m_ActiveScene);

		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::openScene() {
		std::string filepath = FileDialogs::openFile("Mixture Scene (*.mxscene)\0*.mxscene\0");
		if (filepath.empty()) return;

		openScene(filepath);
	}

	void EditorLayer::openScene(const std::filesystem::path& path) {
		if (m_SceneState != SceneState::Play)
			onSceneStop();

		if (path.extension().string() != ".mxscene") {
			MX_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = createRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.deserialize(path.string())) {
			m_EditorScene = newScene;
			m_EditorScene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.setContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::saveScene() {
		if (!m_EditorScenePath.empty())
			serializeScene(m_ActiveScene, m_EditorScenePath);
		else
			saveSceneAs();
	}

	void EditorLayer::serializeScene(Ref<Scene> scene, const std::filesystem::path& path) {
		SceneSerializer serializer(scene);
		serializer.serialize(path.string());
	}

	void EditorLayer::saveSceneAs() {
		std::string filepath = FileDialogs::saveFile("Mixture Scene (*.mxscene)\0*.mxscene\0");
		if (filepath.empty()) return;

		serializeScene(m_ActiveScene, filepath);
		m_EditorScenePath = filepath;
	}

	void EditorLayer::onScenePlay() {
		if (m_SceneState == SceneState::Simulate) onSceneStop();

		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::copy(m_EditorScene);
		m_ActiveScene->onRuntimeStart();

		m_SceneHierarchyPanel.setContext(m_ActiveScene);
	}

	void EditorLayer::onSceneSimulate() {
		if (m_SceneState == SceneState::Play)
			onSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::copy(m_EditorScene);
		m_ActiveScene->onSimulationStart();

		m_SceneHierarchyPanel.setContext(m_ActiveScene);
	}

	void EditorLayer::onSceneStop() {
		//MX_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->onRuntimeStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->onSimulationStop();

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.setContext(m_ActiveScene);
	}

	void EditorLayer::onDuplicateEntity() {
		if (m_SceneState != SceneState::Edit) return;

		Entity selectedEntity = m_SceneHierarchyPanel.getSelectedEntity();
		if (selectedEntity)
			m_EditorScene->duplicateEntity(selectedEntity);
	}
}
