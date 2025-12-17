#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/RHI.hpp"

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Instance.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"
#include "Platform/Vulkan/Device.hpp"

#include <vulkan/vulkan.hpp>
#include <vector>
#include <iostream>
#include <optional>

namespace Mixture::Vulkan
{
    class Context : public RHI::IGraphicsContext
    {
    public:
        Context(const ApplicationDescription& appDescription);
        ~Context();

        RHI::GraphicsAPI GetAPI() const override { return RHI::GraphicsAPI::Vulkan; }

        Ref<Instance> GetInstance() const { return m_Instance; }
        Ref<PhysicalDevice> GetPhysicalDevice() const { return m_PhysicalDevice; }
        Ref<Device> GetDevice() const { return m_Device; }

        static Context& Get();
    private:
        Ref<Instance> m_Instance;
        Ref<PhysicalDevice> m_PhysicalDevice;
        Ref<Device> m_Device;
    };

}
