#pragma once
#include "Mixture/Core/Base.hpp"

#include "Mixture/Renderer/Texture.hpp"
#include "Platform/Vulkan/Image.hpp"

namespace  Mixture::Vulkan
{
    class Texture2D final : public Mixture::Texture2D
    {
    public:
        explicit Texture2D(Jasper::TextureSpecification specification);
        explicit Texture2D(const std::string& path);
        ~Texture2D() override;

        
        void SetData(const void* data, uint32_t size) override;
        void Bind(uint32_t slot = 0) const override;

        OPAL_NODISCARD const Jasper::TextureSpecification& GetSpecification() const override { return m_Specification; }
        OPAL_NODISCARD uint32_t GetWidth() const override { return m_Image->GetExtent().width; }
        OPAL_NODISCARD uint32_t GetHeight() const override { return m_Image->GetExtent().height; }
        OPAL_NODISCARD const void* GetDescriptorInfo() const override { return &m_DescriptorImageInfo; }
        OPAL_NODISCARD bool IsLoaded() const override { return m_Loaded; }

    private:
        Jasper::TextureSpecification m_Specification;
        bool m_Loaded = false;

        VkDescriptorImageInfo m_DescriptorImageInfo{};
        Scope<Image> m_Image;
        Scope<ImageView> m_ImageView;
        Scope<ImageSampler> m_ImageSampler;
        Scope<DeviceMemory> m_ImageMemory;
    };
}
