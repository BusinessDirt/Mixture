#pragma once
#include "Mixture/Core/Base.hpp"

namespace Mixture
{
    enum class ImageFormat
    {
        None = 0, R8, RGB8, RGBA8, RGBA32F    
    };

    struct TextureSpecification
    {
        uint32_t Width = 1;
        uint32_t Height = 1;
        uint32_t MipLevels = 1;
        ImageFormat Format = ImageFormat::RGBA8;
    };
    
    class Texture
    {
    public:
        virtual ~Texture() = default;
        
        virtual void SetData(const void* data, uint32_t size) = 0;
        virtual void Bind(uint32_t slot = 0) const = 0;

        OPAL_NODISCARD virtual const TextureSpecification& GetSpecification() const = 0;
        OPAL_NODISCARD virtual uint32_t GetWidth() const = 0;
        OPAL_NODISCARD virtual uint32_t GetHeight() const = 0;
        OPAL_NODISCARD virtual const void* GetDescriptorInfo() const = 0;
        OPAL_NODISCARD virtual bool IsLoaded() const = 0;
    };

    class Texture2D : public Texture
    {
    public:
        static Ref<Texture2D> Create(const TextureSpecification& specification);
        static Ref<Texture2D> Create(const std::string& path);
    };
}
