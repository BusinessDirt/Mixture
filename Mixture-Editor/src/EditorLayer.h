#pragma once

#include "Mixture.h"
#include "Panels/SceneHierarchyPanel.h"

namespace Mixture {
	class EditorLayer : public Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		void onUpdate(Timestep ts) override;
		virtual void onImGuiRender() override;
		void onEvent(Event& e) override;
	private:
		bool onKeyPressed(KeyPressedEvent& e);

		void newScene();
		void openScene();
		void saveSceneAs();
	private:
		OrthographicCameraController m_CameraController;

		//Temp
		Ref<Framebuffer> m_Framebuffer;
		Ref<Scene> m_ActiveScene;

		EditorCamera m_EditorCamera;
		bool m_PrimaryCamera = true;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		int m_GizmoType = -1;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}
