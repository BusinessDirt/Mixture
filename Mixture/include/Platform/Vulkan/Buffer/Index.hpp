#pragma once

#include "Platform/Vulkan/Base.hpp"
#include "Platform/Vulkan/Buffer/Buffer.hpp"

#include "Mixture/Renderer/Buffer.hpp"

namespace Mixture::Vulkan
{
    class IndexBuffer final : public Mixture::IndexBuffer
    {
    public:
        OPAL_NON_COPIABLE(IndexBuffer);
        
        explicit IndexBuffer(const std::vector<uint32_t>& indices);
        IndexBuffer();
        ~IndexBuffer() override = default;

        void SetData(const std::vector<uint32_t>& indices) override;
        
        void Bind() const override;
        
        OPAL_NODISCARD uint32_t GetCount() const override { return m_IndexCount; }

    private:
        uint32_t m_IndexCount = 0;
        Scope<Buffer> m_IndexBuffer;
    };
}
