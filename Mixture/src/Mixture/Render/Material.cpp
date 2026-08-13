#include "mxpch.hpp"
#include "Mixture/Render/Material.hpp"

namespace Mixture
{
    Material::Material(const std::string& name, UUID id)
        : m_Name(name), m_ID(id)
    {
    }

    Ref<Material> Material::Create(const std::string& name, UUID id)
    {
        return CreateRef<Material>(name, id);
    }

    RHI::IBuffer* Material::GetUniformBuffer(RHI::IGraphicsDevice& device, uint32_t frameIndex)
    {
        const uint32_t bufferIndex = frameIndex % FramesInFlight;
        Ref<RHI::IBuffer>& uniformBuffer = m_UniformBuffers[bufferIndex];
        if (uniformBuffer && m_BufferVersions[bufferIndex] == m_DataVersion)
        {
            return uniformBuffer.get();
        }

        m_DebugName = m_Name + "_UBO";

        RHI::BufferDesc desc;
        desc.Size = sizeof(MaterialData);
        desc.Usage = RHI::BufferUsage::Uniform;
        desc.DebugName = m_DebugName.c_str();

        Ref<RHI::IBuffer> uploadedBuffer = device.CreateBuffer(desc, std::as_bytes(std::span(&m_Data, 1)));
        if (uploadedBuffer)
        {
            // The current frame slot is idle before command recording begins, so replacing
            // its buffer cannot invalidate descriptors from an in-flight frame.
            uniformBuffer = std::move(uploadedBuffer);
            m_BufferVersions[bufferIndex] = m_DataVersion;
        }

        return uniformBuffer.get();
    }
}
