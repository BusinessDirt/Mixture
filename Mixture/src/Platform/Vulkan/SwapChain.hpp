#pragma once

#include "Mixture/Core/Base.hpp"

#include "Platform/Vulkan/Buffer/FrameBuffer.hpp"
#include "Platform/Vulkan/RenderPass.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Mixture::Vulkan
{
    class Semaphore;
    class Fence;

	class SwapChain
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		SwapChain();
		SwapChain(Ref<SwapChain> previous);
		~SwapChain();

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		const FrameBuffer& GetFrameBuffer(int index) { return m_Framebuffers[index]; }
		const RenderPass& GetRenderPass() { return *m_RenderPass; }
		VkImageView GetImageView(int index) { return m_ImageViews[index]; }
		size_t GetImageCount() { return m_Images.size(); }
		VkFormat GetImageFormat() { return m_ImageFormat; }
		VkExtent2D GetExtent() { return m_Extent; }
		uint32_t GetWidth() { return m_Extent.width; }
		uint32_t GetHeight() { return m_Extent.height; }

		float ExtentAspectRatio() 
		{
			return static_cast<float>(m_Extent.width) / static_cast<float>(m_Extent.height);
		}

		VkFormat FindDepthFormat();

		VkResult AcquireNextImage(uint32_t* imageIndex);
		VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

		bool CompareSwapFormats(const SwapChain& swapChain) const
		{
			return swapChain.m_ImageFormat == m_ImageFormat;
		}

	private:
		void Init();

		void CreateSwapChain();
		void CreateImageViews();

		// Helper functions
		VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkFormat m_ImageFormat;
		VkExtent2D m_Extent;

		std::vector<FrameBuffer> m_Framebuffers;
		Scope<RenderPass> m_RenderPass;

		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;

        VULKAN_HANDLE(VkSwapchainKHR, m_SwapChain);
		Ref<SwapChain> m_OldSwapChain;

        // Sync objects
		std::vector<Semaphore> m_ImageAvailableSemaphores;
		std::vector<Semaphore> m_RenderFinishedSemaphores;
		std::vector<Fence> m_InFlightFences;
		std::vector<Fence*> m_ImagesInFlight;
        
		size_t m_CurrentFrame = 0;
	};
}
