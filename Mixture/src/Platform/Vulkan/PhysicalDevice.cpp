#include "mxpch.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Surface.hpp"

#include <stdexcept>

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

    PhysicalDevice::PhysicalDevice(Instance& instance)
    {
        auto devices = instance.GetHandle().enumeratePhysicalDevices();
        if (devices.empty())
            throw std::runtime_error("No Vulkan-capable physical devices were found");

        m_PhysicalDevice = SelectBestDevice(devices);
        m_Indices = FindQueueFamilies(m_PhysicalDevice);

        m_Properties = m_PhysicalDevice.getProperties();
        OPAL_INFO("Core/Vulkan", "Selected GPU: {} ({})", std::string_view(m_Properties.deviceName), m_Properties.deviceType);
        OPAL_INFO("Core/Vulkan", " - API Version: {}", VulkanVersionToString(m_Properties.apiVersion));
        OPAL_INFO("Core/Vulkan", " - Driver Version: {}", VulkanVersionToString(m_Properties.driverVersion));
    }

    std::string_view PhysicalDevice::GetDeviceName() const
    {
        return std::string_view(m_Properties.deviceName);
    }

    bool PhysicalDevice::HasRequiredExtensions(const Vector<vk::ExtensionProperties>& extensions)
    {
        return std::any_of(extensions.begin(), extensions.end(), [](const vk::ExtensionProperties& extension) {
            return std::strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0;
        });
    }

    bool PhysicalDevice::HasRequiredFeatures(
        bool samplerAnisotropy, bool dynamicRendering, bool bufferDeviceAddress)
    {
        return samplerAnisotropy && dynamicRendering && bufferDeviceAddress;
    }

    bool PhysicalDevice::HasUsableSurface(const Vector<vk::SurfaceFormatKHR>& formats,
        const Vector<vk::PresentModeKHR>& presentModes)
    {
        return !formats.empty() && !presentModes.empty();
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
            throw std::runtime_error("No Vulkan device satisfies the required queues, extensions, features, and surface support");

        return bestDevice;
    }

    int PhysicalDevice::RateDeviceSuitability(vk::PhysicalDevice device)
    {
        auto props = device.getProperties();
        vk::PhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures;
        vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures;
        dynamicRenderingFeatures.pNext = &bufferDeviceAddressFeatures;
        vk::PhysicalDeviceFeatures2 features;
        features.pNext = &dynamicRenderingFeatures;
        device.getFeatures2(&features);

        int score = 0;

        QueueFamilyIndices indices = FindQueueFamilies(device);
        if (!indices.IsComplete()) return -1;

        if (props.apiVersion < VK_API_VERSION_1_3)
        {
            OPAL_WARN("Core/Vulkan", "[Skipped] {} does not support Vulkan 1.3", std::string_view(props.deviceName));
            return -1;
        }

        const auto extensions = device.enumerateDeviceExtensionProperties();
        if (!HasRequiredExtensions(extensions)) return -1;
        if (!HasRequiredFeatures(features.features.samplerAnisotropy,
            dynamicRenderingFeatures.dynamicRendering, bufferDeviceAddressFeatures.bufferDeviceAddress)) return -1;

        const auto surface = Context::Get().GetSurface().GetHandle();
        if (!HasUsableSurface(device.getSurfaceFormatsKHR(surface), device.getSurfacePresentModesKHR(surface))) return -1;

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
        vk::SurfaceKHR surface = Context::Get().GetSurface().GetHandle();

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            // Check for Graphics capability
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) indices.Graphics = i;
            if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) indices.Transfer = i;
            if (device.getSurfaceSupportKHR(i, surface)) indices.Present = i;
            if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) indices.Compute = i;

            if (indices.IsComplete()) break;
            i++;
        }

        return indices;
    }
}
