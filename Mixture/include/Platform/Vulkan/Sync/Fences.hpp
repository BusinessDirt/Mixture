#pragma once

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    class Fences
    {
    public:
        Fences(Device& device, uint32_t count, bool signaled);
        ~Fences();

        vk::Fence Get(uint32_t index) const { return m_Handles[index]; }

        vk::Result Wait(uint32_t index);
        vk::Result Reset(uint32_t index);
    private:
        Device* m_Device;
        Vector<vk::Fence> m_Handles;
    };
}
