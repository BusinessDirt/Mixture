#include "Mixture.h"

using namespace Mixture::Window;

class Sandbox : public Mixture::Application {
public:
	Sandbox() {

	}

	~Sandbox() {

	}
};

Mixture::Application* Mixture::CreateApplication() {
	return new Sandbox();
}