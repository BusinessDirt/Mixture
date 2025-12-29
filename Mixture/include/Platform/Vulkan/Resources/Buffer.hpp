#pragma once

/**
 * @file Buffer.hpp
 * @brief Vulkan implementation of the Buffer interface.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/RHI/IBuffer.hpp"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

namespace Mixture::Vulkan
{
    /**
     * @brief Vulkan implementation of a GPU buffer.
     * 
     * Manages a VkBuffer and its memory allocation via VMA.
     */
    class Buffer : public RHI::IBuffer
    {
    public:
        /**
         * @brief Constructs a Vulkan Buffer.
         * 
         * @param desc The buffer description.
         * @param initialData Optional pointer to data to upload on creation.
         */
        Buffer(const RHI::BufferDesc& desc, const void* initialData = nullptr);
        virtual ~Buffer();

        // IBuffer Interface
        virtual uint64_t GetSize() const override { return m_Desc.Size; }
        virtual RHI::BufferUsage GetUsage() const override { return m_Desc.Usage; }

        /**
         * @brief Gets the Vulkan Buffer handle.
         * 
         * @return vk::Buffer The raw handle.
         */
        vk::Buffer GetHandle() const { return m_Buffer; }

    private:
        RHI::BufferDesc m_Desc;
        vk::Buffer m_Buffer = nullptr;
        VmaAllocation m_Allocation = nullptr;
    };
}
