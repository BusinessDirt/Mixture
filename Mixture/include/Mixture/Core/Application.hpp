#pragma once


#include "Mixture/Core/Window.hpp"
#include "Mixture/Core/LayerStack.hpp"
#include "Mixture/Core/AssetManager.hpp"

#include "Mixture/ImGui/ImGuiLayer.hpp"

#include "Mixture/Events/Event.hpp"
#include "Mixture/Events/ApplicationEvent.hpp"


int Entrypoint(int argc, char** argv);

namespace Mixture
{
    struct ApplicationCommandLineArgs
    {
        int Count = 0;
        char** Args = nullptr;

        const char* operator[](const int index) const
        {
            OPAL_ASSERT("Core", index < Count)
            return Args[index];
        }
    };

    class Application
    {
    public:
        OPAL_NON_COPIABLE(Application);
        
        explicit Application(const std::string& name = "Mixture App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
        virtual ~Application();

        void Close();
        
        void OnEvent(Event& event);

        static Application& Get() { return *s_Instance; }
        OPAL_NODISCARD const Window& GetWindow() const { return *m_Window; }
        OPAL_NODISCARD const AssetManager& GetAssetManager() const { return *m_AssetManager; }
        
        void PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }
        void PushOverlay(Layer* layer) { m_LayerStack.PushOverlay(layer); }

    private:
        void Run() const;
        bool OnWindowClose(WindowCloseEvent& e);
        static bool OnFramebufferResize(const FramebufferResizeEvent& e);

    private:
        bool m_Running = true;
        LayerStack m_LayerStack;
        Scope<Window> m_Window;
        Scope<AssetManager> m_AssetManager;
        
    private:
        static Application* s_Instance;
        friend int ::Entrypoint(int argc, char** argv);
    };

    Application* CreateApplication(ApplicationCommandLineArgs args);
}
