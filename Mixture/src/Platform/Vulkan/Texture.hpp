#pragma once

#include "Mixture/Renderer/Texture.hpp"

namespace Mixture::Vulkan
{
    class Image;
    class ImageView;
    class DeviceMemory;
    class Sampler;

    class Texture : public Mixture::Texture
    {
    public:
        Texture(const std::string& filename, const SampledImageInformation& sampler);
        Texture(const SampledImageInformation& sampler, uint32_t width, uint32_t height);
        ~Texture() override;
        
        const Image& GetImage() { return *m_Image; }
        const ImageView& GetImageView() { return *m_ImageView; }
        const Sampler& GetSampler() { return *m_Sampler; }
        
    private:
        Scope<Image> m_Image;
        Scope<ImageView> m_ImageView;
        Scope<DeviceMemory> m_ImageMemory;
        Scope<Sampler> m_Sampler;
    };
}
