#include "Mixture.h"

#include "imgui.h"

using namespace Mixture::Window;

class ExampleLayer : public Mixture::Layer {
public:
	ExampleLayer() : Layer("Example") {}

	void onUpdate() override {
		if (Mixture::Input::isKeyPressed(MX_KEY_TAB))
			MX_TRACE("Tab key is pressed (poll)!");
	}

	virtual void onImGuiRender() override {
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void onEvent(Mixture::Event& event) override {
		if (event.getEventType() == Mixture::EventType::KeyPressed) {
			Mixture::KeyPressedEvent& e = (Mixture::KeyPressedEvent&)event;
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
	}

	~Sandbox() {

	}
};

Mixture::Application* Mixture::CreateApplication() {
	return new Sandbox();
}