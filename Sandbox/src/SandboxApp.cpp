#include "Mixture.h"

using namespace Mixture::Window;

class ExampleLayer : public Mixture::Layer {
public:
	ExampleLayer() : Layer("Example") {}

	void onUpdate() override {
		if (Mixture::Input::isKeyPressed(MX_KEY_TAB))
			MX_TRACE("Tab key is pressed (poll)!");
	}

	void onEvent(Mixture::Events::Event& event) override {
		if (event.getEventType() == Mixture::Events::EventType::KeyPressed) {
			Mixture::Events::KeyPressedEvent& e = (Mixture::Events::KeyPressedEvent&)event;
			if (e.getKeyCode() == MX_KEY_TAB)
				MX_TRACE("Tab key is pressed (event)!");
			MX_TRACE("{0}", (char)e.getKeyCode());
		}
	}
};

class Sandbox : public Mixture::Application {
public:
	Sandbox() {
		pushLayer(new ExampleLayer());
		pushOverlay(new Mixture::ImGui::ImGuiLayer());
	}

	~Sandbox() {

	}
};

Mixture::Application* Mixture::CreateApplication() {
	return new Sandbox();
}