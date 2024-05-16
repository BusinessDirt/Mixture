#include <Mixture.h>
#include <Mixture/Core/EntryPoint.h>
#include "Sandbox2D.h"

class Sandbox : public Mixture::Application {
public:
	Sandbox(Mixture::ApplicationCommandLineArgs args) : Application("Sandbox", args) {
		pushLayer(new Sandbox2D());
	}

	~Sandbox() {

	}
};

Mixture::Application* Mixture::CreateApplication(Mixture::ApplicationCommandLineArgs args) {
	return new Sandbox(args);
}
