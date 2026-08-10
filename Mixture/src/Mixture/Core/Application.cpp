#include "mxpch.hpp"
#include "Mixture/Core/Application.hpp"

#include "Mixture/Core/Time.hpp"
#include "Mixture/Assets/AssetManager.hpp"
#include "Mixture/Core/Threading/TaskSystem.hpp"
#include "Mixture/Render/PipelineCache.hpp"
#include "Mixture/Render/ShaderLibrary.hpp"
#include "Mixture/Render/ImGui/Context.hpp"

#include <Opal/Base.hpp>

#include <stdexcept>

namespace Mixture
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationDescription& appDescription)
        : m_AppDescription(appDescription)
    {
        if (s_Instance)
        {
            throw std::logic_error("Mixture::Application already exists");
        }

        s_Instance = this;
        try
        {
            TaskSystem::Init();

            AssetManager::Get().Init();
            AssetManager::Get().SetAssetRoot("Assets");
            AssetManager::Get().SetGraphicsAPI(appDescription.API);

            auto props = WindowProps();
            props.Title = appDescription.Name;
            props.Width = appDescription.Width;
            props.Height = appDescription.Height;

            m_Window = CreateScope<Window>(props);
            m_Window->SetEventCallback(OPAL_BIND_EVENT_FN(OnEvent));

            m_Context = RHI::IGraphicsContext::Create(appDescription, m_Window->GetNativeWindow());
            if (!m_Context)
            {
                throw std::runtime_error("Failed to create graphics context");
            }

            if (appDescription.EnableImGui)
            {
                m_ImGuiContext = CreateScope<ImGuiContext>(m_Window->GetNativeWindow(), *m_Context);
            }

            PipelineCache::Init(m_Context->GetDevice());
            ShaderLibrary::Init(m_Context->GetDevice());
            m_RenderGraph = CreateScope<RenderGraph>(m_Context->GetDevice());
        }
        catch (...)
        {
            ShutdownOwnedServices();
            throw;
        }
    }

    Application::~Application()
    {
        ShutdownOwnedServices();
    }

    void Application::ShutdownOwnedServices() noexcept
    {
        if (m_Context)
        {
            m_Context->GetDevice().WaitForIdle();
        }

        m_LayerStack.Shutdown();
        m_RenderGraph.reset();
        m_ImGuiContext.reset();

        // Renderer services own device resources and must stop before the device.
        ShaderLibrary::Shutdown();
        PipelineCache::Shutdown();
        m_Context.reset();
        m_Window.reset();

        AssetManager::Get().Shutdown();
        TaskSystem::Shutdown();
        s_Instance = nullptr;
    }

    void Application::Close()
    {
        m_Running = false;
    }

    ImGuiContext& Application::GetImGuiContext() const
    {
        if (!m_ImGuiContext) throw std::logic_error("ImGui is not enabled for this application");
        return *m_ImGuiContext;
    }

    void Application::Run() const
    {
        Timer frameTimer{};

        while (m_Running)
        {
            float timestep = frameTimer.Tick();
            m_Window->OnUpdate();

            // CPU Logic
            for (const auto& layer : m_LayerStack) layer->OnUpdate(timestep);

            m_RenderGraph->Clear();

            if (RHI::ITexture* backbufferTex = m_Context->BeginFrame())
            {
                m_RenderGraph->ImportResource("Backbuffer", backbufferTex);

                if (m_ImGuiContext)
                {
                    m_ImGuiContext->BeginFrame();
                    for (const auto& layer : m_LayerStack) layer->OnDrawImGui();
                    m_ImGuiContext->EndFrame();
                }

                for (const auto& layer : m_LayerStack) layer->OnRender(*m_RenderGraph);

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
