#include "Mixture.hpp"

namespace Mixture
{
    Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		Application* app = new Application("Mixture App", args);
		return app;
	}
}
