#pragma once

#include "Mixture/Core/Base.hpp"

#include "Platform/Vulkan/Buffer/FrameBuffer.hpp"
#include "Platform/Vulkan/Buffer/DepthBuffer.hpp"
#include "Platform/Vulkan/Image/Image.hpp"
#include "Platform/Vulkan/Image/ImageView.hpp"
#include "Platform/Vulkan/DeviceMemory.hpp"
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

		const FrameBuffer& GetFrameBuffer(int index) const { return *m_FrameBuffers[index]; }
		const RenderPass& GetRenderPass() const { return *m_RenderPass; }
		size_t GetImageCount() const { return m_FrameBuffers.size(); }
		VkFormat GetImageFormat() const { return m_FrameBuffers[0]->GetFormat(); }
        VkFormat GetDepthFormat() const { return m_DepthBuffers[0]->GetFormat(); }
		VkExtent2D GetExtent() const { return m_Extent; }
		uint32_t GetWidth() const { return m_Extent.width; }
		uint32_t GetHeight() const { return m_Extent.height; }
        uint32_t GetCurrentFrameIndex() const { return static_cast<uint32_t>(m_CurrentFrame); }

		float ExtentAspectRatio() 
		{
			return static_cast<float>(m_Extent.width) / static_cast<float>(m_Extent.height);
		}

		VkResult AcquireNextImage();
		VkResult SubmitCommandBuffers(const std::vector<CommandBuffer>& buffers);

		bool CompareSwapFormats(const SwapChain& swapChain) const
		{
			return true;
		}

	private:
		void Init();

		void CreateSwapChain();

		// Helper functions
		VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    private:
		VkExtent2D m_Extent;

		Scope<RenderPass> m_RenderPass;
        Vector<Scope<FrameBuffer>> m_FrameBuffers;
        Vector<Scope<DepthBuffer>> m_DepthBuffers;

        VULKAN_HANDLE(VkSwapchainKHR, m_SwapChain);
		Ref<SwapChain> m_OldSwapChain;

        // Sync objects
		Vector<Semaphore> m_ImageAvailableSemaphores;
		Vector<Semaphore> m_RenderFinishedSemaphores;
		Vector<Fence> m_InFlightFences;
		Vector<Fence*> m_ImagesInFlight;
        
		size_t m_CurrentFrame = 0;
	};
}
