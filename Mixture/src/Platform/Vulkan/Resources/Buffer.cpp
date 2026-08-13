#include "mxpch.hpp"
#include "Platform/Vulkan/Resources/Buffer.hpp"
#include "Platform/Vulkan/Resources/AllocationPolicy.hpp"

#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Queue.hpp"
#include "Platform/Vulkan/Context.hpp"

#include <stdexcept>

namespace Mixture::Vulkan
{
    Buffer::Buffer(Ref<Device> device, const RHI::BufferDesc& desc, std::span<const std::byte> initialData)
        : m_Device(std::move(device)), m_Desc(desc)
    {
        if (!m_Device) throw std::invalid_argument("Buffer requires an owning device");
        auto allocator = m_Device->GetAllocator();

        // Create the GPU Buffer
        // We generally allocate GPU_ONLY for best performance.
        vk::BufferCreateInfo bufferInfo = {};
        bufferInfo.size = desc.Size;
        bufferInfo.usage = EnumMapper::MapBufferUsage(desc.Usage) |= vk::BufferUsageFlagBits::eTransferDst;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        VmaAllocationCreateInfo allocInfo = AllocationPolicy::DeviceLocal();
        // TODO: For frequent CPU updates (Uniforms), we might want VMA_MEMORY_USAGE_CPU_TO_GPU
        // and avoid staging, but for Vertex/Index, this is best.

        VkBuffer rawBuffer;
        if (vmaCreateBuffer(allocator, reinterpret_cast<VkBufferCreateInfo*>(&bufferInfo), &allocInfo, &rawBuffer, &m_Allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate Vulkan buffer");
        }

        m_Buffer = rawBuffer;

        if (!initialData.empty())
        {
            // Create Staging Buffer (CPU Visible)
            VkBufferCreateInfo stagingInfo = {};
            stagingInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            stagingInfo.size = desc.Size;
            stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            VmaAllocationCreateInfo stagingAllocInfo = AllocationPolicy::Upload();

            VkBuffer stagingBuffer;
            VmaAllocation stagingAlloc;
            if (vmaCreateBuffer(allocator, &stagingInfo, &stagingAllocInfo, &stagingBuffer, &stagingAlloc, nullptr) != VK_SUCCESS)
            {
                vmaDestroyBuffer(allocator, m_Buffer, m_Allocation);
                m_Buffer = nullptr;
                m_Allocation = nullptr;
                throw std::runtime_error("Failed to allocate Vulkan buffer upload staging memory");
            }

            // Map & Copy
            void* mappedData;
            if (vmaMapMemory(allocator, stagingAlloc, &mappedData) != VK_SUCCESS || !mappedData)
            {
                vmaDestroyBuffer(allocator, stagingBuffer, stagingAlloc);
                vmaDestroyBuffer(allocator, m_Buffer, m_Allocation);
                m_Buffer = nullptr;
                m_Allocation = nullptr;
                throw std::runtime_error("Failed to map Vulkan buffer upload staging memory");
            }
            memcpy(mappedData, initialData.data(), initialData.size());
            vmaUnmapMemory(allocator, stagingAlloc);

            const vk::Buffer destination = m_Buffer;
            Context::Get().EnqueueTransferUpload(
                [stagingBuffer, destination, size = desc.Size](vk::CommandBuffer cmd)
                {
                    vk::BufferCopy copyRegion;
                    copyRegion.size = size;
                    cmd.copyBuffer(stagingBuffer, destination, 1, &copyRegion);
                },
                [allocator, stagingBuffer, stagingAlloc]() { vmaDestroyBuffer(allocator, stagingBuffer, stagingAlloc); });
        }
    }

    Buffer::~Buffer()
    {
        if (m_Buffer)
        {
            vmaDestroyBuffer(m_Device->GetAllocator(), m_Buffer, m_Allocation);
        }
    }
}
