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
            PushOverlay(new UILayer());
        }
    };

    Application* CreateApplication(ApplicationCommandLineArgs args)
    {
        ApplicationDescription desc = ApplicationDescription();
        desc.name = "ExampleApp";
        desc.version = "1.0.0";
        desc.api = RHI::GraphicsAPI::Vulkan;
        desc.args = args;

        const auto app = new ExampleApp(desc);
        return app;
    }
}


