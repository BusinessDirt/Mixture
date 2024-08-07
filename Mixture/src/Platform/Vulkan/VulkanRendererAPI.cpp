#include "mxpch.h"
#include "VulkanRendererAPI.h"

#include "Platform/Vulkan/VulkanManager.h"

namespace Mixture
{
    VulkanRendererAPI::~VulkanRendererAPI() 
    {
        vkDestroyInstance(m_Context.Instance, m_Context.Allocator);
    }

    void VulkanRendererAPI::Init(const std::string& applicationName)
    {
        // TODO: custom allocator.
        m_Context.Allocator = nullptr;

        // Setup Vulkan instance.
        VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.pApplicationName = applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Mixture";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        
        // configure layers
        VulkanManager manager{};
        manager.Init();
        
        VkLayerSettingsCreateInfoEXT layerSettingsCreateInfo{};
        layerSettingsCreateInfo.sType = VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT;
        layerSettingsCreateInfo.pNext = nullptr;
        layerSettingsCreateInfo.settingCount = static_cast<uint32_t>(manager.GetLayerSettings().size());
        layerSettingsCreateInfo.pSettings = manager.GetLayerSettings().data();

        VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(manager.GetExtensions().size());
        createInfo.ppEnabledExtensionNames = manager.GetExtensions().data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(manager.GetLayers().size());
        createInfo.ppEnabledLayerNames = manager.GetLayers().data();
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR; // required for MoltenVK to work
        createInfo.pNext = &layerSettingsCreateInfo;

        VkResult result = vkCreateInstance(&createInfo, m_Context.Allocator, &m_Context.Instance);
        MX_VK_ASSERT(result, "Failed to create VkInstance");
    }

    void VulkanRendererAPI::OnWindowResize(uint32_t width, uint32_t height)
    {
        
    }

    bool VulkanRendererAPI::BeginFrame()
    {
        return true;
    }

    bool VulkanRendererAPI::EndFrame()
    {
        return true;
    }
}
