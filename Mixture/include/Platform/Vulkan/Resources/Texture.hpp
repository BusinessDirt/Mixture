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
        /**
         * @brief Constructs a Standard Texture (memory owned by this class).
         * 
         * @param spec The texture description.
         * @param data Optional initial data to upload.
         */
        Texture(const RHI::TextureDesc& spec, const void* data = nullptr);

        /**
         * @brief Constructs a wrapper around an existing Vulkan Image (e.g., Swapchain Image).
         * 
         * @param format The image format.
         * @param image The existing image handle.
         * @param imageView The existing image view handle.
         * @param width The width of the image.
         * @param height The height of the image.
         */
        Texture(vk::Format format, vk::Image image, vk::ImageView imageView, uint32_t width, uint32_t height);

        virtual ~Texture();

        // ITexture Interface
        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        RHI::Format GetFormat() const override { return m_Format; }
        std::string_view GetDebugName() const override { return m_DebugName; }

        /**
         * @brief Gets the Vulkan Image handle.
         * 
         * @return vk::Image The image handle.
         */
        vk::Image GetImage() const { return m_Image; }

        /**
         * @brief Gets the Vulkan ImageView handle.
         * 
         * @return vk::ImageView The image view handle.
         */
        vk::ImageView GetImageView() const { return m_ImageView; }

        /**
         * @brief Creates a descriptor image info structure for this texture.
         * 
         * @return vk::DescriptorImageInfo The descriptor info.
         */
        vk::DescriptorImageInfo GetDescriptorInfo() const;

        /**
         * @brief Invalidates and releases the texture resources (if owned).
         */
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
