#pragma once

#include "Platform/Vulkan/Base.hpp"
#include "Platform/Vulkan/Buffer/Buffer.hpp"

#include "Mixture/Renderer/Buffer.hpp"

namespace Mixture::Vulkan
{
    class UniformBuffer final : public Mixture::UniformBuffer
    {
    public:
        OPAL_NON_COPIABLE(UniformBuffer);
        
        explicit UniformBuffer(size_t instanceSize, uint32_t instanceCount);
        ~UniformBuffer() override;

        void SetData(const void* data, uint32_t index) const override;
        const void* GetDescriptorInfo(uint32_t index) override;

    private:
        Scope<Buffer> m_Buffer;
        VkDescriptorBufferInfo m_DescriptorInfo;
    };
}
