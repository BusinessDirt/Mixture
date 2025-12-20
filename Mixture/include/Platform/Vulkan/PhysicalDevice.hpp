#pragma once

/**
 * @file PhysicalDevice.hpp
 * @brief Vulkan physical device wrapper.
 */

#include "Mixture/Core/Base.hpp"

#include "Platform/Vulkan/Definitions.hpp"

namespace Mixture::Vulkan
{
    /**
     * @brief Wrapper around a Vulkan physical device (GPU).
     * 
     * Selects the best available GPU and queries its features/queues.
     */
    class PhysicalDevice
    {
    public:
        // Pass the Instance so we can enumerate GPUs
        /**
         * @brief Constructor.
         * 
         * @param instance The Vulkan instance.
         */
        PhysicalDevice(vk::Instance instance);
        ~PhysicalDevice() = default;

        /**
         * @brief Gets the Vulkan physical device handle.
         * 
         * @return vk::PhysicalDevice The raw handle.
         */
        vk::PhysicalDevice GetHandle() const { return m_PhysicalDevice; }

        // Helpers for the Logical Device
        /**
         * @brief Gets the indices of the queue families.
         * 
         * @return QueueFamilyIndices The indices.
         */
        QueueFamilyIndices GetQueueFamilies() const { return m_Indices; }

        /**
         * @brief Gets the name of the device.
         * 
         * @return std::string_view The device name.
         */
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
