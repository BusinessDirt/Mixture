#include "mxpch.hpp"
#include "Mixture/Core/Application.hpp"

#include "Mixture/Core/Time.hpp"
#include "Mixture/Assets/AssetManager.hpp"
#include "Mixture/Render/PipelineCache.hpp"
#include "Mixture/Render/ShaderLibrary.hpp"

#include <Opal/Base.hpp>

namespace Mixture
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationDescription& appDescription)
        : m_AppDescription(appDescription)
    {
        OPAL_ASSERT("Core", !s_Instance, "Mixture::Application::Application() - Application already exists!")

        s_Instance = this;

        auto props = WindowProps();
        props.Title = appDescription.Name;
        props.Width = appDescription.Width;
        props.Height = appDescription.Height;

        m_Window = CreateScope<Window>(props);
        m_Window->SetEventCallback(OPAL_BIND_EVENT_FN(OnEvent));

        m_Context = RHI::IGraphicsContext::Create(appDescription, m_Window->GetNativeWindow());
        m_RenderGraph = CreateScope<RenderGraph>(m_Context->GetDevice());

        AssetManager::Get().Init();
        AssetManager::Get().SetAssetRoot("Assets");
        AssetManager::Get().SetGraphicsAPI(appDescription.API);
    }

    Application::~Application()
    {
        m_Context->GetDevice().WaitForIdle();
        m_LayerStack.Shutdown();
        m_RenderGraph.reset();
        m_Context.reset();
    }

    void Application::Close()
    {
        m_Running = false;
    }

    void Application::Run() const
    {
        Timer frameTimer{};

        while (m_Running)
        {
            float timestep = frameTimer.Tick();
            m_Window->OnUpdate();

            // CPU Logic
            for (Layer* layer : m_LayerStack) layer->OnUpdate(timestep);

            m_RenderGraph->Clear();

            if (RHI::ITexture* backbufferTex = m_Context->BeginFrame())
            {
                m_RenderGraph->ImportResource("Backbuffer", backbufferTex);

                for (Layer* layer : m_LayerStack) layer->OnRender(*m_RenderGraph);

                m_RenderGraph->Compile();

                if (auto commandList = m_Context->GetCommandBuffer())
                {
                    commandList->Begin();
                    m_RenderGraph->Execute(commandList.get(), m_Context.get());
                    commandList->End();
                }

                m_Context->EndFrame();
            }
        }
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(OPAL_BIND_EVENT_FN(OnWindowClose));
        dispatcher.Dispatch<FramebufferResizeEvent>(OPAL_BIND_EVENT_FN(OnFramebufferResize));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (event.Handled) break;
            (*it)->OnEvent(event);
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnFramebufferResize(const FramebufferResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0) {
            // Minimized
            return false;
        }

        // Tell the backend to resize
        if (m_Context) m_Context->OnResize(e.GetWidth(), e.GetHeight());
        return false;
    }
}
