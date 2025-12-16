#pragma once

#include "Mixture/Core/Window.hpp"
#include "Mixture/Core/LayerStack.hpp"

#include "Mixture/Render/Graph/RenderGraph.hpp"
#include "Mixture/Render/RHI/RHI.hpp"

#include "Mixture/Events/Event.hpp"
#include "Mixture/Events/ApplicationEvent.hpp"


int Entrypoint(int argc, char** argv);

namespace Mixture
{
    /**
     * @brief Structure to hold command line arguments passed to the application.
     */
    struct ApplicationCommandLineArgs
    {
        int Count = 0;
        char** Args = nullptr;

        /**
         * @brief Operator to access arguments by index.
         *
         * @param index The index of the argument.
         * @return const char* The argument string.
         */
        const char* operator[](const int index) const
        {
            OPAL_ASSERT("Core", index < Count)
            return Args[index];
        }
    };

    struct ApplicationDescription
    {
        std::string name = "Mixture App";
        std::string version = "1.0.0";
        RHI::GraphicsAPI api = RHI::GraphicsAPI::None;

        ApplicationCommandLineArgs args = ApplicationCommandLineArgs();
    };

    /**
     * @brief The main application class.
     *
     * Manages the main run loop, window, layer stack, and events.
     */
    class Application
    {
    public:
        OPAL_NON_COPIABLE(Application);

        /**
         * @brief Constructor.
         *
         * @param name The name of the application.
         * @param args Command line arguments.
         */
        explicit Application(const ApplicationDescription& appDescription = ApplicationDescription());
        virtual ~Application();

        /**
         * @brief Closes the application.
         */
        void Close();

        /**
         * @brief Handles events.
         *
         * @param event The event to handle.
         */
        void OnEvent(Event& event);

        /**
         * @brief Gets the singleton application instance.
         *
         * @return Application& Reference to the application instance.
         */
        static Application& Get() { return *s_Instance; }

        /**
         * @brief Gets the application window.
         *
         * @return const Window& Reference to the window.
         */
        OPAL_NODISCARD const Window& GetWindow() const { return *m_Window; }

        /**
         * @brief Pushes a layer onto the layer stack.
         *
         * @param layer The layer to push.
         */
        void PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }

        /**
         * @brief Pushes an overlay onto the layer stack.
         *
         * @param layer The overlay to push.
         */
        void PushOverlay(Layer* layer) { m_LayerStack.PushOverlay(layer); }

    private:
        void Run() const;
        bool OnWindowClose(WindowCloseEvent& e);
        static bool OnFramebufferResize(const FramebufferResizeEvent& e);

    private:
        ApplicationDescription m_AppDescription;

        bool m_Running = true;
        LayerStack m_LayerStack;

        Scope<Window> m_Window;
        Scope<RenderGraph> m_RenderGraph;
        Scope<RHI::IGraphicsContext> m_Context;
    private:
        static Application* s_Instance;
        friend int ::Entrypoint(int argc, char** argv);
    };

    /**
     * @brief Factory function to create the application instance.
     *
     * Must be implemented by the client application.
     *
     * @param args Command line arguments.
     * @return Application* Pointer to the created application.
     */
    Application* CreateApplication(ApplicationCommandLineArgs args);
}
