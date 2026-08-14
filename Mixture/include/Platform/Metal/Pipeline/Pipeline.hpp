#pragma once
#if defined(OPAL_PLATFORM_DARWIN)

/**
 * @file Pipeline.hpp
 * @brief Metal implementation of the Pipeline interface.
 */

#include "Platform/Metal/Definitions.hpp"
#include "Mixture/Render/RHI/IPipeline.hpp"
#include "Mixture/Assets/Shaders/SlangShaderReflector.hpp"

#include <string>
#include <vector>

namespace Mixture::Metal
{
    class Device;

    /**
     * @brief Metal implementation of a graphics pipeline.
     */
    class Pipeline : public RHI::IPipeline
    {
    public:
        Pipeline(Ref<Device> device, const RHI::PipelineDesc& desc);
        ~Pipeline();

        bool IsValid() const override { return m_PipelineState != nullptr; }

        const ShaderReflectionData* GetShaderReflectionData() const override { return &m_ReflectionData; }

        MTL::RenderPipelineState* GetPipelineState() const { return m_PipelineState; }
        MTL::DepthStencilState* GetDepthStencilState() const { return m_DepthStencilState; }
        MTL::CullMode GetCullMode() const { return m_CullMode; }
        MTL::Winding GetWinding() const { return m_Winding; }

        const ShaderReflectionData::PushConstant* FindPushConstant(RHI::ShaderStage stage, uint32_t size) const;
        const ShaderReflectionData::Resource* FindResource(const std::string& name) const;
        const ShaderReflectionData::Resource* FindUniformBuffer(uint32_t binding, uint32_t set = 0) const;
        const ShaderReflectionData::Resource* FindTexture(uint32_t binding, uint32_t set = 0) const;
        const ShaderReflectionData::Resource* FindSampler(uint32_t binding, uint32_t set = 0) const;
        const ShaderReflectionData::Resource* FindStorageBuffer(uint32_t binding, uint32_t set = 0) const;

        const Vector<ShaderReflectionData::Resource>& GetUniformBuffers() const { return m_ReflectionData.UniformBuffers; }
        const Vector<ShaderReflectionData::Resource>& GetStorageBuffers() const { return m_ReflectionData.StorageBuffers; }
        const Vector<ShaderReflectionData::Resource>& GetTextures() const { return m_ReflectionData.Textures; }
        const Vector<ShaderReflectionData::Resource>& GetStorageImages() const { return m_ReflectionData.StorageImages; }
        const Vector<ShaderReflectionData::Resource>& GetSamplers() const { return m_ReflectionData.Samplers; }
        const Vector<ShaderReflectionData::PushConstant>& GetPushConstants() const { return m_ReflectionData.PushConstants; }
        const Vector<ShaderReflectionData::VertexAttribute>& GetVertexAttributes() const { return m_ReflectionData.InputAttributes; }

    private:
        void ReflectPipeline(const RHI::PipelineDesc& desc);

        Ref<Device> m_Device;
        MTL::RenderPipelineState* m_PipelineState = nullptr;
        MTL::DepthStencilState* m_DepthStencilState = nullptr;
        MTL::CullMode m_CullMode = MTL::CullModeNone;
        MTL::Winding m_Winding = MTL::WindingCounterClockwise;
        ShaderReflectionData m_ReflectionData;
    };
}

#endif
