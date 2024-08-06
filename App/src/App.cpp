#include "Mixture/Core/Entrypoint.h"

namespace Mixture
{
    Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		Application* app = new Application("Application", args);
		return app;
	}
}