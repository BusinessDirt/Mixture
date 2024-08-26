#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/Shaders/ShaderInformation.hpp"
#include "Mixture/Renderer/Buffer/UniformBuffer.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"
#include "Mixture/Renderer/Pipeline/GraphicsPipeline.hpp"

#include "Platform/Vulkan/Buffer/Buffer.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class UniformBuffer : public Mixture::UniformBuffer
    {
    public:
        MX_NON_COPIABLE(UniformBuffer);
        
        UniformBuffer(uint32_t size);
        UniformBuffer();
        ~UniformBuffer();
        
        void Update(void* data) override;
        
    private:
        std::vector<Scope<Buffer>> m_Buffers;
    };
}
