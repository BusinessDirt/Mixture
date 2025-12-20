#include "mxpch.hpp"
#include "Platform/Vulkan/Resources/Buffer.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    namespace
    {
        namespace Utils
        {
            static vk::BufferUsageFlags GetVulkanBufferUsage(RHI::BufferUsage usage)
            {
                vk::BufferUsageFlags flags = vk::BufferUsageFlagBits::eTransferDst; // Always allow updates via staging

                switch (usage)
                {
                    case RHI::BufferUsage::Vertex:   flags |= vk::BufferUsageFlagBits::eVertexBuffer; break;
                    case RHI::BufferUsage::Index:    flags |= vk::BufferUsageFlagBits::eIndexBuffer; break;
                    case RHI::BufferUsage::Uniform:  flags |= vk::BufferUsageFlagBits::eUniformBuffer; break;
                    case RHI::BufferUsage::Storage:  flags |= vk::BufferUsageFlagBits::eStorageBuffer; break;
                    case RHI::BufferUsage::TransferSrc: flags |= vk::BufferUsageFlagBits::eTransferSrc; break;
                    case RHI::BufferUsage::TransferDst: flags |= vk::BufferUsageFlagBits::eTransferDst; break;
                }
                return flags;
            }

            static void CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size)
            {
                auto device = Context::Get().GetLogicalDevice();
                auto commandPool = Context::Get().GetCommandPool();
                auto queue = device->GetGraphicsQueue(); // Or TransferQueue if you have one

                // Allocate temporary command buffer
                vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
                vk::CommandBuffer cmd = device->GetHandle().allocateCommandBuffers(allocInfo)[0];

                // Record Copy Command
                vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
                cmd.begin(beginInfo);

                vk::BufferCopy copyRegion;
                copyRegion.size = size;
                cmd.copyBuffer(src, dst, 1, &copyRegion);

                cmd.end();

                // Submit and Wait
                vk::SubmitInfo submitInfo;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &cmd;

                queue.submit(submitInfo, nullptr);
                queue.waitIdle(); // Stall CPU until copy is done

                device->GetHandle().freeCommandBuffers(commandPool, cmd);
            }
        }

        Buffer::Buffer(const RHI::BufferDesc& desc, const void* initialData)
            : m_Desc(desc)
        {
            auto allocator = Context::Get().GetLogicalDevice()->GetAllocator();

            // Create the GPU Buffer
            // We generally allocate GPU_ONLY for best performance.
            vk::BufferCreateInfo bufferInfo = {};
            bufferInfo.size = desc.Size;
            bufferInfo.usage = Utils::GetVulkanBufferUsage(desc.Usage);
            bufferInfo.sharingMode = vk::SharingMode::eExclusive;

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
            // TODO: For frequent CPU updates (Uniforms), you might want VMA_MEMORY_USAGE_CPU_TO_GPU
            // and avoid staging, but for Vertex/Index, this is best.

            VkBuffer rawBuffer;
            if (vmaCreateBuffer(allocator, reinterpret_cast<VkBufferCreateInfo*>(&bufferInfo), &allocInfo, &rawBuffer, &m_Allocation, nullptr) != VK_SUCCESS)
            {
                OPAL_ERROR("Core/Vulkan", "Failed to allocate Buffer!");
                return;
            }

            m_Buffer = rawBuffer;

            if (initialData)
            {
                // Create Staging Buffer (CPU Visible)
                VkBufferCreateInfo stagingInfo = {};
                stagingInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                stagingInfo.size = desc.Size;
                stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

                VmaAllocationCreateInfo stagingAllocInfo = {};
                stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
                stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

                VkBuffer stagingBuffer;
                VmaAllocation stagingAlloc;
                vmaCreateBuffer(allocator, &stagingInfo, &stagingAllocInfo, &stagingBuffer, &stagingAlloc, nullptr);

                // Map & Copy
                void* mappedData;
                vmaMapMemory(allocator, stagingAlloc, &mappedData);
                memcpy(mappedData, initialData, desc.Size);
                vmaUnmapMemory(allocator, stagingAlloc);

                Utils::CopyBuffer(stagingBuffer, m_Buffer, desc.Size);
                vmaDestroyBuffer(allocator, stagingBuffer, stagingAlloc);
            }
        }

        Buffer::~Buffer()
        {
            if (m_Buffer) {
                vmaDestroyBuffer(Context::Get().GetLogicalDevice()->GetAllocator(), m_Buffer, m_Allocation);
            }
        }
    }
}
