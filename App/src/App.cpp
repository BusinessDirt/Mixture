#include "Mixture.hpp"

namespace Mixture
{
    class ExampleApp : public Application
    {
    public:
        ExampleApp(ApplicationCommandLineArgs args) : Application("Example App", args)
        {
            Renderer::PushRendererSystem(new RendererSystem());
        }
        
        ~ExampleApp()
        {
            
        }
    };

    Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ExampleApp* app = new ExampleApp(args);
		return app;
	}
}
