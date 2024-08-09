#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class Manager
    {
    public:
        Manager();
        
        void Init();
        
        const std::vector<const char*>& GetLayers() const { return m_Layers; }
        const std::vector<const char*>& GetInstanceExtensions() const { return m_InstanceExtensions; }
        const std::vector<const char*>& GetDeviceExtensions() const { return m_DeviceExtensions; }
        const std::vector<VkLayerSettingEXT>& GetLayerSettings() const { return m_LayerSettings; }
        
        bool CheckLayerSupport();
        bool CheckInstanceExtensionSupport();
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
        
    private:
        std::vector<VkLayerProperties> GetAvailableLayers() const;
        std::vector<VkExtensionProperties> GetAvailableInstanceExtensions() const;
        std::vector<VkExtensionProperties> GetAvailableDeviceExtensions(VkPhysicalDevice device) const;
        
        void GetRequiredLayers();
        void GetRequiredInstanceExtensions();
        void GetRequiredDeviceExtensions();
        
    private:
        std::vector<const char*> m_Layers{};
        std::vector<const char*> m_InstanceExtensions{};
        std::vector<const char*> m_DeviceExtensions{};
        std::vector<VkLayerSettingEXT> m_LayerSettings{};
    };
}
