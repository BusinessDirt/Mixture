#include "Mixture.hpp"

namespace Mixture
{
    Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		Application* app = new Application("Application", args);
		return app;
	}
}