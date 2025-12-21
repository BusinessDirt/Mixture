#pragma once
#include "Mixture/Core/Base.hpp"

#include "Platform/Vulkan/Definitions.hpp"

namespace Mixture::Vulkan
{
    class Semaphores
    {
    public:
        Semaphores(vk::Device device, uint32_t count);
        ~Semaphores();

        vk::Semaphore Get(uint32_t index) const { return m_Handles[index]; }
    private:
        vk::Device m_Device;
        Vector<vk::Semaphore> m_Handles;
    };
}
