#include "mxpch.hpp"
#include "Viewport.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Image/Sampler.hpp"
#include "Platform/Vulkan/Command/SingleTimeCommand.hpp"

#include <backends/imgui_impl_vulkan.h>

namespace Mixture::Vulkan
{
    Viewport::Viewport(VkExtent2D extent)
        : m_Extent(extent)
    {
        m_Texture = CreateScope<Texture>(SampledImageInformation{}, extent.width, extent.height);
        m_TextureSet = ImGui_ImplVulkan_AddTexture(m_Texture->GetSampler().GetHandle(), m_Texture->GetImageView().GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    Viewport::~Viewport()
    {
        ImGui_ImplVulkan_RemoveTexture(m_TextureSet);
        m_Texture = nullptr;
    }

    void Viewport::Update(VkImage swapChainImage)
    {
        SingleTimeCommand::Submit([&](VkCommandBuffer commandBuffer)
            {
                // Transition the swap chain image to TRANSFER_SRC_OPTIMAL
                VkImageMemoryBarrier barrier = {};
                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.image = swapChainImage;
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.levelCount = 1;
                barrier.subresourceRange.layerCount = 1;

                vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                // Transition the off-screen image to TRANSFER_DST_OPTIMAL
                barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.image = m_Texture->GetImage().GetHandle();

                vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     0, 0, nullptr, 0, nullptr, 1, &barrier);
            
                // Copy the swap chain image to the off-screen image
                VkImageCopy copyRegion = {};
                copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
                copyRegion.srcOffset = { 0, 0, 0 };
                copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
                copyRegion.dstOffset = { 0, 0, 0 };
                copyRegion.extent = { m_Extent.width, m_Extent.height, 1 };

                vkCmdCopyImage(commandBuffer, swapChainImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               m_Texture->GetImage().GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
            
                // Transition the off-screen image to SHADER_READ_ONLY_OPTIMAL
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                barrier.image = m_Texture->GetImage().GetHandle();

                vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                     0, 0, nullptr, 0, nullptr, 1, &barrier);
            });
    }
}
