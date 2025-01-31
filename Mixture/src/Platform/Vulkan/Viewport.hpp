#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/Texture.hpp"

namespace Mixture::Vulkan
{
    class Viewport
    {
    public:
        MX_NON_COPIABLE(Viewport);
        
        Viewport(VkExtent2D extent);
        ~Viewport();
        
        void Update(VkImage swapChainImage) const;
        
        VkDescriptorSet GetDescriptorSet() const { return m_TextureSet; }
        
    private:
        VkExtent2D m_Extent;
        Scope<Texture> m_Texture;
        VkDescriptorSet m_TextureSet;
    };
}
