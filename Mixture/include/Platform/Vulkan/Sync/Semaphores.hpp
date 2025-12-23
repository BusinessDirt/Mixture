#pragma once

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    class Semaphores
    {
    public:
        Semaphores(Device& device, uint32_t count);
        ~Semaphores();

        vk::Semaphore Get(uint32_t index) const { return m_Handles[index]; }
    private:
        Device* m_Device;
        Vector<vk::Semaphore> m_Handles;
    };
}
