#pragma once

/**
 * @file Texture.hpp
 * @brief Vulkan implementation of the Texture interface.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/ITexture.hpp"

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

namespace Mixture::Vulkan
{
    /**
     * @brief Vulkan implementation of a GPU texture.
     *
     * Handles both standard textures (owned memory) and wrapped external textures (e.g. Swapchain images).
     */
    class Texture : public RHI::ITexture
    {
    public:
        //Standard Texture (Created from spec/file) - We OWN this memory
        Texture(const RHI::TextureDesc& spec);
        Texture(const std::string& path);

        // Swapchain/External Wrapper - We DO NOT own this memory
        Texture(vk::Format format, vk::Image image, vk::ImageView imageView, uint32_t width, uint32_t height);

        virtual ~Texture();

        // ITexture Interface
        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        RHI::Format GetFormat() const override { return m_Format; }
        std::string_view GetDebugName() const override { return m_DebugName; }

        // Vulkan Specific Getters (Used by CommandList / ImGui)
        vk::Image GetImage() const { return m_Image; }
        vk::ImageView GetImageView() const { return m_ImageView; }

        // Helper for Descriptors (Future use)
        vk::DescriptorImageInfo GetDescriptorInfo() const;

        // Lifecycle (For Standard Textures)
        void Invalidate();

    private:
        void Release(); // Helper to clean up

    private:
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        RHI::Format m_Format;
        std::string_view m_DebugName;

        // Vulkan Handles
        vk::Image m_Image = nullptr;
        vk::ImageView m_ImageView = nullptr;
        vk::Sampler m_Sampler = nullptr; // Optional: Standard textures usually have a sampler

        // Memory Management
        VmaAllocation m_Allocation = nullptr;
        bool m_OwnsImage = false; // <--- The Critical Flag
    };

}
