#include <Mixture.h>
#include <Mixture/Core/EntryPoint.h>
#include "Sandbox2D.h"

class Sandbox : public Mixture::Application {
public:
	Sandbox() {
		pushLayer(new Sandbox2D());
	}

	~Sandbox() {

	}
};

Mixture::Application* Mixture::CreateApplication() {
	return new Sandbox();
}