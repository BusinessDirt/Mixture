#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Buffer/IndexBuffer.hpp"

#include "Platform/Vulkan/Buffer/Buffer.hpp"

namespace Mixture::Vulkan
{
    class IndexBuffer : public Mixture::IndexBuffer
    {
    public:
        IndexBuffer(const std::vector<uint16_t>& indices);
        ~IndexBuffer() override;
        
        void Bind(CommandBuffer commandBuffer) override;
        uint32_t GetIndexCount() override { return m_IndexCount; }
        
    private:
        uint32_t m_IndexCount = 0;
        Scope<Buffer> m_IndexBuffer;
    };
}
