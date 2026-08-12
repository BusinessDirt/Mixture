#include "mxpch.hpp"
#include "Mixture/Render/ImGui/Context.hpp"
#include "Mixture/Render/RenderStats.hpp"

#include "Mixture/Render/RHI/IGraphicsContext.hpp"
#include "Platform/Vulkan/Command/List.hpp"
#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Instance.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"
#include "Platform/Vulkan/Queue.hpp"
#include "Platform/Vulkan/Swapchain.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

#include <GLFW/glfw3.h>
#include <stdexcept>

namespace Mixture
{
    struct ImGuiContext::Impl
    {
        Vulkan::Context* GraphicsContext = nullptr;
        VkFormat ColorFormat = VK_FORMAT_UNDEFINED;
        VkPipelineRenderingCreateInfo PipelineInfo{};
        bool PlatformInitialized = false;
        bool RendererInitialized = false;
        bool FrameActive = false;
    };

    ImGuiContext::ImGuiContext(void* windowHandle, RHI::IGraphicsContext& graphicsContext)
        : m_Impl(CreateScope<Impl>())
    {
        if (!windowHandle) throw std::invalid_argument("ImGui requires a valid window handle");
        if (graphicsContext.GetAPI() != RHI::GraphicsAPI::Vulkan)
            throw std::invalid_argument("ImGui is not implemented for the selected graphics API");

        m_Impl->GraphicsContext = dynamic_cast<Vulkan::Context*>(&graphicsContext);
        if (!m_Impl->GraphicsContext)
            throw std::invalid_argument("Vulkan ImGui integration requires a Vulkan context");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        try
        {
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

            ImGui::StyleColorsDark();

            if (!ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(windowHandle), true))
                throw std::runtime_error("Failed to initialize the ImGui GLFW backend");
            m_Impl->PlatformInitialized = true;

            auto& context = *m_Impl->GraphicsContext;
            auto& swapchain = context.GetSwapchain();
            if (swapchain.GetImageCount() < 2)
                throw std::runtime_error("ImGui requires at least two swapchain images");
            m_Impl->ColorFormat = static_cast<VkFormat>(swapchain.GetImageFormat());

            m_Impl->PipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
            m_Impl->PipelineInfo.colorAttachmentCount = 1;
            m_Impl->PipelineInfo.pColorAttachmentFormats = &m_Impl->ColorFormat;

            ImGui_ImplVulkan_InitInfo initInfo{};
            initInfo.ApiVersion = VK_API_VERSION_1_3;
            initInfo.Instance = context.GetInstance().GetHandle();
            initInfo.PhysicalDevice = context.GetPhysicalDevice().GetHandle();
            initInfo.Device = context.GetLogicalDevice().GetHandle();
            initInfo.QueueFamily = context.GetGraphicsQueue().GetFamilyIndex();
            initInfo.Queue = context.GetGraphicsQueue().GetHandle();
            initInfo.DescriptorPoolSize = 100;
            initInfo.MinImageCount = 2;
            initInfo.ImageCount = swapchain.GetImageCount();
            initInfo.UseDynamicRendering = true;
            initInfo.PipelineInfoMain.PipelineRenderingCreateInfo = m_Impl->PipelineInfo;
            initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
            initInfo.CheckVkResultFn = [](VkResult result)
            {
                if (result < VK_SUCCESS)
                    OPAL_ERROR("Core/ImGui", "Vulkan backend error: {}", static_cast<int>(result));
            };

            if (!ImGui_ImplVulkan_Init(&initInfo))
                throw std::runtime_error("Failed to initialize the ImGui Vulkan backend");
            m_Impl->RendererInitialized = true;
        }
        catch (...)
        {
            if (m_Impl->RendererInitialized) ImGui_ImplVulkan_Shutdown();
            if (m_Impl->PlatformInitialized) ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            throw;
        }
    }

    ImGuiContext::~ImGuiContext()
    {
        if (!m_Impl) return;
        if (m_Impl->FrameActive) ImGui::EndFrame();
        if (m_Impl->RendererInitialized) ImGui_ImplVulkan_Shutdown();
        if (m_Impl->PlatformInitialized) ImGui_ImplGlfw_Shutdown();
        if (ImGui::GetCurrentContext()) ImGui::DestroyContext();
    }

    void ImGuiContext::BeginFrame()
    {
        if (m_Impl->FrameActive) throw std::logic_error("An ImGui frame is already active");
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        m_Impl->FrameActive = true;
    }

    void ImGuiContext::EndFrame()
    {
        if (!m_Impl->FrameActive) throw std::logic_error("No ImGui frame is active");
        ImGui::Render();
        m_Impl->FrameActive = false;
    }

    void ImGuiContext::Render(RHI::ICommandList* commandList) const
    {
        if (m_Impl->FrameActive) throw std::logic_error("ImGui draw data has not been finalized");
        auto* vulkanCommandList = dynamic_cast<Vulkan::CommandList*>(commandList);
        if (!vulkanCommandList)
            throw std::invalid_argument("ImGui Vulkan rendering requires a Vulkan command list");

        ImDrawData* drawData = ImGui::GetDrawData();
        if (drawData)
        {
            for (int n = 0; n < drawData->CmdListsCount; n++)
            {
                const ImDrawList* cmdList = drawData->CmdLists[n];
                for (int cmdi = 0; cmdi < cmdList->CmdBuffer.Size; cmdi++)
                {
                    const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmdi];
                    if (!pcmd->UserCallback)
                    {
                        RenderStats::Get().RecordDrawIndexed(pcmd->ElemCount, 1);
                    }
                }
            }
        }

        ImGui_ImplVulkan_RenderDrawData(drawData, vulkanCommandList->GetGraphicsCommandBuffer());
    }
}
