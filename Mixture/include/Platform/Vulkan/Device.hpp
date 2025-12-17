#pragma once

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"

#include <memory>

namespace Mixture::Vulkan
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