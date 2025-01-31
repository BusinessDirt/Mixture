#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class Manager;

    class Instance
    {
    public:
        MX_NON_COPIABLE(Instance);
        
        Instance(const std::string& applicationName, const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions);
        ~Instance();
        
    private:
        VULKAN_HANDLE(VkInstance, m_Instance);
    };
}
