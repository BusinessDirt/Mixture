#pragma once
#include "Mixture/Core/Base.h"
#include "Mixture/Core/Application.h"

extern Mixture::Application* Mixture::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv) {
	Mixture::Log::Init();

	MX_PROFILE_BEGIN_SESSION("Startup", "MixtureProfile-Startup.json");
	Mixture::Application* app = Mixture::CreateApplication({ argc, argv});
	MX_PROFILE_END_SESSION();

	MX_PROFILE_BEGIN_SESSION("Runtime", "MixtureProfile-Runtime.json");
	app->run();
	MX_PROFILE_END_SESSION();

	MX_PROFILE_BEGIN_SESSION("Shutdown", "MixtureProfile-Shutdown.json");
	delete app;
	MX_PROFILE_END_SESSION();
}
