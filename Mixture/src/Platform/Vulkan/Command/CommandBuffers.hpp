#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class CommandBuffers
    {
    public:
        MX_NON_COPIABLE(CommandBuffers);
        
        CommandBuffers(size_t size);
        ~CommandBuffers();
        
        CommandBuffer GetCurrent() const;
        CommandBuffer Get(int i) const { return m_CommandBuffers[i]; }
    private:
        std::vector<VkCommandBuffer> m_CommandBuffers;
    };
}
