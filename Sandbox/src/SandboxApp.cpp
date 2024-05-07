#include "Mixture.h"

using namespace Mixture::Window;

class Sandbox : public Mixture::Application {
public:
	Sandbox(const Window::Properties& properties) : Application(properties) {

	}

	~Sandbox() {

	}
};

Mixture::Application* Mixture::CreateApplication() {
	Window::Window::Properties props;
	props.title = "Sandbox";

	return new Sandbox(props);
}