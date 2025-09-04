#include "mxpch.hpp"
#include "Platform/Vulkan/Buffer/Depth.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/DeviceMemory.hpp"
#include "Platform/Vulkan/Image.hpp"

namespace Mixture::Vulkan
{
	namespace Util
	{
		namespace
		{
			VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, const VkImageTiling tiling, const VkFormatFeatureFlags features)
			{
				for (const auto format : candidates)
				{
					VkFormatProperties props;
					vkGetPhysicalDeviceFormatProperties(Context::PhysicalDevice->GetHandle(), format, &props);

					if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) 
						return format;

					if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
						return format;
				}

				OPAL_CORE_ERROR("Mixture::Vulkan::Util::FindSupportedFormat() - Failed to find supported format!");
				return VK_FORMAT_UNDEFINED;
			}
		}
	}

	DepthBuffer::DepthBuffer(const VkExtent2D extent)
		: m_Format(FindDepthFormat())
	{
		m_Image.reset(new Image(extent, m_Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT));
		m_ImageMemory.reset(new DeviceMemory(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
		m_ImageView.reset(new ImageView(m_Image->GetHandle(), m_Format, VK_IMAGE_ASPECT_DEPTH_BIT));

		m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	DepthBuffer::~DepthBuffer()
	{
		m_ImageView = nullptr;
		m_Image = nullptr;
		m_ImageMemory = nullptr;
	}

	VkFormat DepthBuffer::FindDepthFormat()
	{
		return Util::FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}
}
