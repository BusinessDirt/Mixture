#pragma once

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"

#include "Mixture/Render/RHI/IGraphicsDevice.hpp"

#include <memory>

namespace Mixture::Vulkan
{
	class Device : public RHI::IGraphicsDevice
	{
	public:
		Device(Ref<PhysicalDevice> physicalDevice);
		~Device();

		vk::Device GetHandle() const { return m_Device; }
		vk::Queue GetGraphicsQueue() const { return m_GraphicsQueue; }

        Ref<RHI::IShader> CreateShader(const std::string& filepath, RHI::ShaderStage stage) override;
        Ref<RHI::IBuffer> CreateBuffer(const RHI::BufferDesc& desc) override;
        Ref<RHI::ITexture> CreateTexture(const RHI::TextureDesc& desc) override;
        Ref<RHI::IPipeline> CreatePipeline(const RHI::PipelineDesc& desc) override;

		void WaitForIdle() override { m_Device.waitIdle(); }

	private:
		Ref<PhysicalDevice> m_PhysicalDevice;
		vk::Device m_Device;

		vk::Queue m_GraphicsQueue;
	};
}
