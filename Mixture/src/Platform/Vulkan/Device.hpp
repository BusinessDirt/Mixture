#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class Manager;
    class PhysicalDevice;

    class Device
    {
    public:
        MX_NON_COPIABLE(Device);
        
        Device(const Manager& manager);
        ~Device();
        
    private:
        VULKAN_HANDLE(VkDevice, m_Device);
        
        uint32_t m_GraphicsQueueIndex;
        VkQueue m_GraphicsQueue{VK_NULL_HANDLE};
        
        uint32_t m_PresentQueueIndex;
        VkQueue m_PresentQueue{VK_NULL_HANDLE};
    };
}
