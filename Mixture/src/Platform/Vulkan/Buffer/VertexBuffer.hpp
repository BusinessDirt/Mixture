#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Buffer/VertexBuffer.hpp"

#include "Platform/Vulkan/Buffer/Buffer.hpp"

namespace Mixture::Vulkan 
{
    class VertexBuffer : public Mixture::VertexBuffer
    {
    public:
        VertexBuffer(const std::vector<Vertex>& vertices);
        ~VertexBuffer() override;
        
        void Bind(CommandBuffer commandBuffer) override;
        uint32_t GetVertexCount() override { return m_VertexCount; }
        
    private:
        uint32_t m_VertexCount = 0;
        Scope<Buffer> m_VertexBuffer;
    };
}
