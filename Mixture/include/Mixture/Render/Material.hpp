#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/IAsset.hpp"
#include "Mixture/Render/RHI/IBuffer.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"
#include <glm/glm.hpp>
#include <array>
#include <cstdint>
#include <string>

namespace Mixture
{
    /**
     * @brief PBR Material parameters layout aligned for GPU uniform buffers.
     */
    struct alignas(16) MaterialData
    {
        glm::vec4 AlbedoColor{ 1.0f, 1.0f, 1.0f, 1.0f };
        float Metallic = 0.0f;
        float Roughness = 0.5f;
        glm::vec2 Tiling{ 1.0f, 1.0f };
        alignas(16) glm::vec3 EmissionColor{ 0.0f, 0.0f, 0.0f };
        float _Pad0 = 0.0f;
        float EmissionIntensity = 1.0f;
        float _Pad1[3] = { 0.0f, 0.0f, 0.0f };
    };

    static_assert(sizeof(MaterialData) == 64, "MaterialData must be 64 bytes to match GPU uniform buffer layout");
    static_assert(offsetof(MaterialData, AlbedoColor) == 0);
    static_assert(offsetof(MaterialData, Metallic) == 16);
    static_assert(offsetof(MaterialData, Roughness) == 20);
    static_assert(offsetof(MaterialData, Tiling) == 24);
    static_assert(offsetof(MaterialData, EmissionColor) == 32);
    static_assert(offsetof(MaterialData, EmissionIntensity) == 48);

    /**
     * @brief Material asset encapsulating shading parameters, uniform buffer, and texture maps.
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
        MaterialData& GetData() { MarkDirty(); return m_Data; }
        const MaterialData& GetData() const { return m_Data; }

        const glm::vec4& GetAlbedoColor() const { return m_Data.AlbedoColor; }
        void SetAlbedoColor(const glm::vec4& color) { m_Data.AlbedoColor = color; MarkDirty(); }

        float GetMetallic() const { return m_Data.Metallic; }
        void SetMetallic(float metallic) { m_Data.Metallic = metallic; MarkDirty(); }

        float GetRoughness() const { return m_Data.Roughness; }
        void SetRoughness(float roughness) { m_Data.Roughness = roughness; MarkDirty(); }

        const glm::vec3& GetEmissionColor() const { return m_Data.EmissionColor; }
        void SetEmissionColor(const glm::vec3& color) { m_Data.EmissionColor = color; MarkDirty(); }

        float GetEmissionIntensity() const { return m_Data.EmissionIntensity; }
        void SetEmissionIntensity(float intensity) { m_Data.EmissionIntensity = intensity; MarkDirty(); }

        const glm::vec2& GetTiling() const { return m_Data.Tiling; }
        void SetTiling(const glm::vec2& tiling) { m_Data.Tiling = tiling; MarkDirty(); }

        // Texture Map Paths
        const std::string& GetAlbedoMapPath() const { return m_AlbedoMapPath; }
        void SetAlbedoMapPath(const std::string& path) { m_AlbedoMapPath = path; }

        const std::string& GetNormalMapPath() const { return m_NormalMapPath; }
        void SetNormalMapPath(const std::string& path) { m_NormalMapPath = path; }

        const std::string& GetMetallicRoughnessMapPath() const { return m_MetallicRoughnessMapPath; }
        void SetMetallicRoughnessMapPath(const std::string& path) { m_MetallicRoughnessMapPath = path; }

        /**
         * @brief Returns or creates the GPU Uniform Buffer for this material.
         */
        RHI::IBuffer* GetUniformBuffer(RHI::IGraphicsDevice& device, uint32_t frameIndex);

    private:
        static constexpr uint32_t FramesInFlight = 2;
        void MarkDirty() { ++m_DataVersion; }

        UUID m_ID;
        std::string m_Name;
        std::string m_DebugName;
        MaterialData m_Data;
        uint64_t m_DataVersion = 1;
        std::array<Ref<RHI::IBuffer>, FramesInFlight> m_UniformBuffers;
        std::array<uint64_t, FramesInFlight> m_BufferVersions{};

        std::string m_AlbedoMapPath;
        std::string m_NormalMapPath;
        std::string m_MetallicRoughnessMapPath;
    };
}
