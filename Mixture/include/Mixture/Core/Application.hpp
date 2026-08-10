#pragma once

/**
 * @file Application.hpp
 * @brief Defines the Application class and entry point.
 */

#include "Mixture/Core/Window.hpp"
#include "Mixture/Core/LayerStack.hpp"

#include "Mixture/Render/Graph/RenderGraph.hpp"
#include "Mixture/Render/RHI/RHI.hpp"

#include "Mixture/Events/Event.hpp"
#include "Mixture/Events/ApplicationEvent.hpp"

#include <stdexcept>


int Entrypoint(int argc, char** argv);

namespace Mixture
{
    class ImGuiContext;

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
            if (index < 0 || index >= Count || !Args)
                throw std::out_of_range("Application command-line argument index is out of range");
            return Args[index];
        }
    };

    struct ApplicationDescription
    {
        std::string Name = "Mixture App";
        uint32_t Width = 1280;
        uint32_t Height = 720;

        std::string Version = "1.0.0";
        RHI::GraphicsAPI API = RHI::GraphicsAPI::None;
        bool EnableImGui = false;

        ApplicationCommandLineArgs Args = ApplicationCommandLineArgs();
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
        static Application& Get()
        {
            if (!s_Instance) throw std::logic_error("No Mixture application exists");
            return *s_Instance;
        }

        /** @brief Returns whether an application currently owns engine services. */
        static bool IsCreated() { return s_Instance != nullptr; }

        /**
         * @brief Gets the application window.
         *
         * @return const Window& Reference to the window.
         */
        OPAL_NODISCARD const Window& GetWindow() const { return *m_Window; }

        /**
         * @brief Gets the applications graphics context.
         *
         * @return const RHI::IGraphicsContext& Reference to the graphics context.
         */
        OPAL_NODISCARD const RHI::IGraphicsContext& GetContext() const { return *m_Context; }

        /** Returns the optional application-owned ImGui integration. */
        OPAL_NODISCARD ImGuiContext& GetImGuiContext() const;

        /**
         * @brief Pushes a layer onto the layer stack.
         *
         * @param layer The layer to push.
         */
        void PushLayer(Scope<Layer> layer) { m_LayerStack.PushLayer(std::move(layer)); }

        /**
         * @brief Pushes an overlay onto the layer stack.
         *
         * @param layer The overlay to push.
         */
        void PushOverlay(Scope<Layer> layer) { m_LayerStack.PushOverlay(std::move(layer)); }

    private:
        void Run() const;
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnFramebufferResize(const FramebufferResizeEvent& e);
        void ShutdownOwnedServices() noexcept;

    private:
        ApplicationDescription m_AppDescription;

        bool m_Running = true;
        LayerStack m_LayerStack;

        Scope<Window> m_Window;
        Scope<RHI::IGraphicsContext> m_Context;
        Scope<ImGuiContext> m_ImGuiContext;
        Scope<RenderGraph> m_RenderGraph;
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
