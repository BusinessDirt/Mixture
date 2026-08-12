#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/IAsset.hpp"
#include <glm/glm.hpp>
#include <string>

namespace Mixture
{
    /**
     * @brief PBR Material parameters and properties.
     */
    struct MaterialData
    {
        glm::vec4 AlbedoColor{ 1.0f, 1.0f, 1.0f, 1.0f };
        float Metallic = 0.0f;
        float Roughness = 0.5f;
        glm::vec2 Tiling{ 1.0f, 1.0f };
        glm::vec3 EmissionColor{ 0.0f, 0.0f, 0.0f };
        float EmissionIntensity = 1.0f;
    };

    /**
     * @brief Material asset encapsulating shading parameters and texture maps.
     */
    class Material : public IAsset
    {
    public:
        Material(const std::string& name = "Default Material", UUID id = UUID());
        ~Material() override = default;

        static Ref<Material> Create(const std::string& name = "Default Material", UUID id = UUID());

        // IAsset interface implementation
        UUID GetID() const override { return m_ID; }
        AssetType GetType() const override { return AssetType::Material; }
        const std::string& GetName() const override { return m_Name; }
        size_t GetMemoryUsage() const override { return sizeof(MaterialData) + m_Name.capacity(); }

        void SetName(const std::string& name) { m_Name = name; }

        // Material Data Getters & Setters
        MaterialData& GetData() { return m_Data; }
        const MaterialData& GetData() const { return m_Data; }

        const glm::vec4& GetAlbedoColor() const { return m_Data.AlbedoColor; }
        void SetAlbedoColor(const glm::vec4& color) { m_Data.AlbedoColor = color; }

        float GetMetallic() const { return m_Data.Metallic; }
        void SetMetallic(float metallic) { m_Data.Metallic = metallic; }

        float GetRoughness() const { return m_Data.Roughness; }
        void SetRoughness(float roughness) { m_Data.Roughness = roughness; }

        const glm::vec3& GetEmissionColor() const { return m_Data.EmissionColor; }
        void SetEmissionColor(const glm::vec3& color) { m_Data.EmissionColor = color; }

        float GetEmissionIntensity() const { return m_Data.EmissionIntensity; }
        void SetEmissionIntensity(float intensity) { m_Data.EmissionIntensity = intensity; }

        const glm::vec2& GetTiling() const { return m_Data.Tiling; }
        void SetTiling(const glm::vec2& tiling) { m_Data.Tiling = tiling; }

        // Texture Map Paths
        const std::string& GetAlbedoMapPath() const { return m_AlbedoMapPath; }
        void SetAlbedoMapPath(const std::string& path) { m_AlbedoMapPath = path; }

        const std::string& GetNormalMapPath() const { return m_NormalMapPath; }
        void SetNormalMapPath(const std::string& path) { m_NormalMapPath = path; }

        const std::string& GetMetallicRoughnessMapPath() const { return m_MetallicRoughnessMapPath; }
        void SetMetallicRoughnessMapPath(const std::string& path) { m_MetallicRoughnessMapPath = path; }

    private:
        UUID m_ID;
        std::string m_Name;
        MaterialData m_Data;

        std::string m_AlbedoMapPath;
        std::string m_NormalMapPath;
        std::string m_MetallicRoughnessMapPath;
    };
}
