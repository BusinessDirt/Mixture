#include "mxpch.hpp"
#include "Mixture/Core/Application.hpp"

#include "Mixture/Core/Time.hpp"
#include "Mixture/Core/Version.hpp"
#include "Mixture/Core/Threading/TaskSystem.hpp"
#include "Mixture/Assets/AssetManager.hpp"
#include "Mixture/Render/PipelineCache.hpp"
#include "Mixture/Render/ShaderLibrary.hpp"
#include "Mixture/Render/ImGui/Context.hpp"
#include "Mixture/Render/RenderStats.hpp"

#if !defined(OPAL_DIST)
#include "Platform/Vulkan/Device.hpp"
#if defined(__APPLE__)
#include <mach/mach.h>
#elif defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#elif defined(__linux__)
#include <unistd.h>
#include <fstream>
#endif
#endif

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

        OPAL_INFO("Core", "Successfully initialized Mixture v{0}", GetVersionString());
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

    void Application::Run() const
    {
        Timer frameTimer{};

        while (m_Running)
        {
            float timestep = frameTimer.Tick();
            float frameTimeMs = timestep * 1000.0f;
            float fps = timestep > 0.0f ? 1.0f / timestep : 0.0f;
            RenderStats::Get().UpdateFrameTiming(frameTimeMs, fps);

#if !defined(OPAL_DIST)
            float ramMB = 0.0f;
            // TODO: abstract this into PlatformUtils or something similar
#if defined(__APPLE__)
            struct mach_task_basic_info info{};
            mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
            if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
            {
                ramMB = static_cast<float>(info.resident_size) / (1024.0f * 1024.0f);
            }
#elif defined(_WIN32)
            PROCESS_MEMORY_COUNTERS pmc{};
            if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
            {
                ramMB = static_cast<float>(pmc.WorkingSetSize) / (1024.0f * 1024.0f);
            }
#elif defined(__linux__)
            long pages = 0;
            std::ifstream statm("/proc/self/statm");
            if (statm >> pages >> pages)
            {
                const long pageSize = sysconf(_SC_PAGESIZE);
                ramMB = static_cast<float>(pages * pageSize) / (1024.0f * 1024.0f);
            }
#endif

            float vramMB = 0.0f;
            if (m_Context)
            {
                if (auto* vkDevice = dynamic_cast<const Vulkan::Device*>(&m_Context->GetDevice()))
                {
                    if (VmaAllocator allocator = vkDevice->GetAllocator())
                    {
                        VmaTotalStatistics vmaStats{};
                        vmaCalculateStatistics(allocator, &vmaStats);
                        vramMB = static_cast<float>(vmaStats.total.statistics.allocationBytes) / (1024.0f * 1024.0f);
                    }
                }
            }
            RenderStats::Get().SetMemoryUsage(vramMB, ramMB);
#endif // !defined(OPAL_DIST)

            m_Window->OnUpdate();

            // CPU Logic
            for (const auto& layer : m_LayerStack) layer->OnUpdate(timestep);

            m_RenderGraph->Clear();

            if (RHI::ITexture* backbufferTex = m_Context->BeginFrame())
            {
                m_RenderGraph->ImportResource("SwapchainBackbuffer", backbufferTex);
                m_RenderGraph->AddAlias("Backbuffer", "SwapchainBackbuffer");

                if (m_ImGuiContext)
                {
                    m_ImGuiContext->BeginFrame();
                    for (const auto& layer : m_LayerStack) layer->OnDrawImGui();
                    m_ImGuiContext->EndFrame();
                }

                for (const auto& layer : m_LayerStack) layer->OnPreRender(*m_RenderGraph);
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
