#include "mxpch.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    namespace
    {
        std::string VulkanVersionToString(uint32_t version)
        {
            std::stringstream ss;
            ss << VK_API_VERSION_MAJOR(version) << "."
                << VK_API_VERSION_MINOR(version) << "."
                << VK_API_VERSION_PATCH(version);
            return ss.str();
        }
    }

    PhysicalDevice::PhysicalDevice(vk::Instance instance)
    {
        auto devices = instance.enumeratePhysicalDevices();
        if (devices.empty())
        {
            OPAL_CRITICAL("Core/Vulkan", "Failed to find GPUs with Vulkan support!");
            exit(-1);
        }

        m_PhysicalDevice = SelectBestDevice(devices);
        m_Indices = FindQueueFamilies(m_PhysicalDevice);

        auto selectedProps = m_PhysicalDevice.getProperties();
        OPAL_INFO("Core/Vulkan", "Selected GPU: {} ({})", std::string_view(selectedProps.deviceName), selectedProps.deviceType);
        OPAL_INFO("Core/Vulkan", " - API Version: {}", VulkanVersionToString(selectedProps.apiVersion));
        OPAL_INFO("Core/Vulkan", " - Driver Version: {}", VulkanVersionToString(selectedProps.driverVersion));
    }

    std::string_view PhysicalDevice::GetDeviceName() const
    {
        return std::string_view(m_PhysicalDevice.getProperties().deviceName);
    }

    vk::PhysicalDevice PhysicalDevice::SelectBestDevice(const Vector<vk::PhysicalDevice>& devices)
    {
        vk::PhysicalDevice bestDevice = nullptr;
        int bestScore = -1;

        for (const auto& device : devices)
        {
            int score = RateDeviceSuitability(device);
            if (score > bestScore)
            {
                bestDevice = device;
                bestScore = score;
            }
        }

        if (!bestDevice || bestScore < 0)
        {
            OPAL_CRITICAL("Core/Vulkan", "No suitable GPU found!");
            exit(-1);
        }

        return bestDevice;
    }

    int PhysicalDevice::RateDeviceSuitability(vk::PhysicalDevice device)
    {
        auto props = device.getProperties();
        auto features = device.getFeatures();

        int score = 0;

        QueueFamilyIndices indices = FindQueueFamilies(device);
        if (!indices.IsComplete()) return -1;

        if (props.apiVersion < VK_API_VERSION_1_3)
        {
            OPAL_WARN("Core/Vulkan", "[Skipped] {} does not support Vulkan 1.3", GetDeviceName());
            return -1;
        }

        if (!features.samplerAnisotropy) return -1;

        // Big Score for Discrete GPU (Dedicated Card)
        if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) score += 1000;

        // Bonus for higher max texture size (e.g. 4096 vs 16384)
        score += props.limits.maxImageDimension2D;

        return score;
    }

    QueueFamilyIndices PhysicalDevice::FindQueueFamilies(vk::PhysicalDevice device)
    {
        QueueFamilyIndices indices;
        Vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();
        vk::SurfaceKHR surface = Context::Get().GetSurface()->GetHandle();

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            // Check for Graphics capability
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) indices.Graphics = i;
            if (device.getSurfaceSupportKHR(i, surface)) indices.Present = i;
            if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) indices.Compute = i;

            if (indices.IsComplete()) break;
            i++;
        }

        return indices;
    }
}
