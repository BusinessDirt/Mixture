#pragma once

#include <Opal/Base.hpp>

namespace Jasper
{
    enum class ImageFormat : uint8_t
    {
        None = 0, R8, RGB8, RGBA8, RGBA32F
    };

    struct TextureSpecification
    {
        uint32_t Width = 1;
        uint32_t Height = 1;
        uint32_t MipLevels = 1;
        ImageFormat Format = ImageFormat::RGBA8;
        std::string Path;
    };

    using TextureHandle = uint32_t;

    namespace Util
    {
        inline const char* ImageFormatToString(const ImageFormat format)
        {
            switch (format)
            {
                case ImageFormat::R8: return "R8";
                case ImageFormat::RGB8: return "RGB8";
                case ImageFormat::RGBA8: return "RGBA8";
                case ImageFormat::RGBA32F: return "RGBA32F";
                case ImageFormat::None: return "None";
            }
            
            return "None";
        }
    }
    
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
}