#include <Mixture.hpp>
#include <Mixture/Core/Entrypoint.hpp>

#include "MainLayer.hpp"
#include "UILayer.hpp"

namespace Mixture
{
    class ExampleApp final : public Application
    {
    public:
        explicit ExampleApp(ApplicationCommandLineArgs args)
            : Application("App", args)
        {
            PushLayer(new MainLayer());
            PushOverlay(new UILayer());
        }
    };

    Application* CreateApplication(ApplicationCommandLineArgs args)
    {
        const auto app = new ExampleApp(args);
        return app;
    }
}


