#pragma once
#include "Jasper/Textures/Texture.hpp"
#include "Mixture/Core/Base.hpp"

namespace Mixture
{
    class Texture2D : public Jasper::Texture
    {
    public:
        static Ref<Texture2D> Create(const Jasper::TextureSpecification& specification);
        static Ref<Texture2D> Load(const std::string& path);
    };
}
