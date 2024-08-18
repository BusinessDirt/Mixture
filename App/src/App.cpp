#include "Mixture/Core/Entrypoint.hpp"
#include "Mixture.hpp"

#include "MainLayer.hpp"

namespace Mixture
{
    class EditorApp : public Application
    {
    public:
        EditorApp(ApplicationCommandLineArgs args) 
            : Application("App", args)
        {
            Renderer::PushLayer(new MainLayer());
        }
    };

    Application* CreateApplication(ApplicationCommandLineArgs args)
	{
        EditorApp* app = new EditorApp(args);
		return app;
	}
}
