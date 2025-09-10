#pragma once
#include "Mixture/Core/Base.hpp"

namespace Mixture
{
    class GraphicsPipeline
    {
    public:
        virtual ~GraphicsPipeline() = default;

        virtual void Bind() const = 0;
        virtual void PushConstants(const void* pValues) const = 0;
        virtual void UpdateGlobalUniformBuffer(const void* bufferInfo) const = 0;
        virtual void UpdateTexture(uint32_t textureHandle) const = 0;

        static Ref<GraphicsPipeline> Create(const std::string& shaderName);
    };
}
