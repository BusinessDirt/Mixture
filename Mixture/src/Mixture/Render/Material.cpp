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

    RHI::IBuffer* Material::GetUniformBuffer(RHI::IGraphicsDevice& device)
    {
        if (!m_UniformBuffer)
        {
            m_DebugName = m_Name + "_UBO";

            RHI::BufferDesc desc;
            desc.Size = sizeof(MaterialData);
            desc.Usage = RHI::BufferUsage::Uniform;
            desc.DebugName = m_DebugName.c_str();

            m_UniformBuffer = device.CreateBuffer(desc, std::as_bytes(std::span(&m_Data, 1)));
            m_IsDirty = false;
        }

        return m_UniformBuffer.get();
    }
}
