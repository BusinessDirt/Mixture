#include "mxpch.hpp"
#include "VulkanContext.hpp"

namespace Mixture
{
    VulkanContext* VulkanContext::s_Instance = nullptr;
    std::mutex VulkanContext::s_Mutex;

    VulkanContext& VulkanContext::Get()
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        if (s_Instance == nullptr) s_Instance = new VulkanContext();
        return *s_Instance;
    }
}
