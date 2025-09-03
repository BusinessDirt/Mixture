#pragma once
#include "Mixture/Core/Base.hpp"

#include "Mixture/Renderer/Renderer.hpp"

namespace Mixture
{
    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void SetData(const void* data, uint32_t size, uint32_t count) = 0;

        OPAL_NODISCARD virtual uint32_t GetCount() const = 0;
        
        static Ref<VertexBuffer> Create(const void* data, uint32_t size, size_t count);
    };

    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void SetData(const std::vector<uint32_t>& indices) = 0;
        
        OPAL_NODISCARD virtual uint32_t GetCount() const = 0;

        static Ref<IndexBuffer> Create(const std::vector<uint32_t>& indices);
    };

    class UniformBuffer
    {
    public:
        virtual ~UniformBuffer() = default;
        
        virtual void SetData(const void* data, uint32_t index = 0) const = 0;
        virtual const void* GetDescriptorInfo(uint32_t index = 0) = 0;

        static Ref<UniformBuffer> Create(size_t instanceSize, uint32_t instanceCount = 1);
    };
}
