#include "Mixture.h"

using namespace Mixture::Window;

class ExampleLayer : public Mixture::Layer {
public:
	ExampleLayer() : Layer("Example") {}

	void onUpdate() override {
		//MX_INFO("ExampleLayer::onUpdate");
	}

	void onEvent(Mixture::Events::Event& event) override {
		//MX_TRACE("{0}", event.toString());
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