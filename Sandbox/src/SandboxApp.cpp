#include <Mixture.h>
#include <Mixture/Core/EntryPoint.h>

#include "Mixture/Platform/OpenGL/OpenGLShader.h"

#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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