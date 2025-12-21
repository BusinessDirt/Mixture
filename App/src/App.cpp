#include <Mixture.hpp>
#include <Mixture/Core/Entrypoint.hpp>

#include "MainLayer.hpp"
#include "UILayer.hpp"

namespace Mixture
{
    class ExampleApp final : public Application
    {
    public:
        explicit ExampleApp(const ApplicationDescription& appDescription)
            : Application(appDescription)
        {
            PushLayer(new MainLayer());
            //PushOverlay(new UILayer());
        }
    };

    Application* CreateApplication(ApplicationCommandLineArgs args)
    {
        ApplicationDescription desc = ApplicationDescription();
        desc.Name = "ExampleApp";
        desc.Width = 1280;
        desc.Height = 720;
        desc.Version = "1.0.0";
        desc.API = RHI::GraphicsAPI::Vulkan;
        desc.Args = args;

        const auto app = new ExampleApp(desc);
        return app;
    }
}


