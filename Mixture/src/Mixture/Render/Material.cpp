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
}
