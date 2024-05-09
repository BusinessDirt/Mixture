#pragma once

#include "Mixture/Layer.h"

#include "Mixture/Events/ApplicationEvent.h"
#include "Mixture/Events/KeyEvent.h"
#include "Mixture/Events/MouseEvent.h"

namespace Mixture::ImGui {
	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void onAttach();
		void onDetach();
		void onUpdate();
		void onEvent(Events::Event& event);
	private:
		bool onMouseButtonPressedEvent(Events::MouseButtonPressedEvent& e);
		bool onMouseButtonReleasedEvent(Events::MouseButtonReleasedEvent& e);
		bool onMouseMovedEvent(Events::MouseMovedEvent& e);
		bool onMouseScrolledEvent(Events::MouseScrolledEvent& e);
		bool onKeyPressedEvent(Events::KeyPressedEvent& e);
		bool onKeyReleasedEvent(Events::KeyReleasedEvent& e);
		bool onKeyTypedEvent(Events::KeyTypedEvent& e);
		bool onWindowResizeEvent(Events::WindowResizeEvent& e);

	private:
		float m_Time = 0.0f;
	};
}