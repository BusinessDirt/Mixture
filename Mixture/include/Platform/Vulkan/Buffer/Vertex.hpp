#pragma once

#include "Mixture/Renderer/Buffer.hpp"
#include "Platform/Vulkan/Buffer/Buffer.hpp"

namespace Mixture::Vulkan
{
    class VertexBuffer final : public Mixture::VertexBuffer
    {
    public:
        OPAL_NON_COPIABLE(VertexBuffer);

        explicit VertexBuffer(const void* data, uint32_t size, uint32_t count);
        ~VertexBuffer() override = default;

        void SetData(const void* data, uint32_t size, uint32_t count) override;
            
        void Bind() const override;
            
        OPAL_NODISCARD uint32_t GetCount() const override { return m_VertexCount; }

    private:
        uint32_t m_VertexCount = 0;
        Scope<Buffer> m_VertexBuffer;
    };
}

