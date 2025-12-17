#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/RHI.hpp"

#include "Platform/Vulkan/VulkanDefinitions.hpp"
#include "Platform/Vulkan/Instance.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"
#include "Platform/Vulkan/Device.hpp"

#include <vulkan/vulkan.hpp>
#include <vector>
#include <iostream>
#include <optional>

namespace Mixture
{
    class VulkanContext : public RHI::IGraphicsContext
    {
    public:
        VulkanContext(const ApplicationDescription& appDescription);
        ~VulkanContext();

        RHI::GraphicsAPI GetAPI() const override { return RHI::GraphicsAPI::Vulkan; }

        Ref<Instance> GetInstance() const { return m_Instance; }
        Ref<PhysicalDevice> GetPhysicalDevice() const { return m_PhysicalDevice; }
        Ref<Device> GetDevice() const { return m_Device; }

        static VulkanContext& Get();
    private:
        Ref<Instance> m_Instance;
        Ref<PhysicalDevice> m_PhysicalDevice;
        Ref<Device> m_Device;
    };

}
