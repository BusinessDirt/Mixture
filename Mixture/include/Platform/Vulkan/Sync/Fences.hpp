#pragma once
#include "Mixture/Core/Base.hpp"

#include "Platform/Vulkan/Definitions.hpp"

namespace Mixture::Vulkan
{
    class Fences
    {
    public:
        Fences(vk::Device device, uint32_t count, bool signaled);
        ~Fences();

        vk::Fence Get(uint32_t index) const { return m_Handles[index]; }

        vk::Result Wait(uint32_t index);
        vk::Result Reset(uint32_t index);
    private:
        vk::Device m_Device;
        Vector<vk::Fence> m_Handles;
    };
}
