#pragma once
#include "Mixture/Core/Base.hpp"

#include "Platform/Vulkan/VulkanDefinitions.hpp"

namespace Mixture
{
    class PhysicalDevice
    {
    public:
        // Pass the Instance so we can enumerate GPUs
        PhysicalDevice(vk::Instance instance);
        ~PhysicalDevice() = default;

        vk::PhysicalDevice GetHandle() const { return m_PhysicalDevice; }

        // Helpers for the Logical Device
        QueueFamilyIndices GetQueueFamilies() const { return m_Indices; }
        std::string_view GetDeviceName() const;

    private:
        vk::PhysicalDevice SelectBestDevice(const Vector<vk::PhysicalDevice>& devices);
        int RateDeviceSuitability(vk::PhysicalDevice device);
        QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);

    private:
        vk::PhysicalDevice m_PhysicalDevice;
        QueueFamilyIndices m_Indices;
    };
}
