#pragma once

extern Mixture::Application* Mixture::CreateApplication();

int main(int argc, char** argv) {
	Mixture::Log::Init();
	MX_CORE_INFO("Initialized Core Log!");
	MX_INFO("Initialized Client Log!");

	Mixture::Application* app = Mixture::CreateApplication();
	app->run();
	delete app;
}