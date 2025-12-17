#pragma once

#include "Platform/Vulkan/VulkanDefinitions.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"

#include <memory>

namespace Mixture
{
	class Device
	{
	public:
		Device(Ref<PhysicalDevice> physicalDevice);
		~Device();

		vk::Device GetHandle() const { return m_Device; }
		vk::Queue GetGraphicsQueue() const { return m_GraphicsQueue; }

		void WaitIdle() const { m_Device.waitIdle(); }

	private:
		Ref<PhysicalDevice> m_PhysicalDevice;
		vk::Device m_Device;

		vk::Queue m_GraphicsQueue;
	};
}