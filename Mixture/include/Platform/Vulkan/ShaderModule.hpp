#pragma once

#include "Platform/Vulkan/Base.hpp"
#include "Jasper.hpp"

namespace Mixture::Vulkan
{
	class ShaderModule
	{
	public:
		OPAL_NON_COPIABLE(ShaderModule);
		
		ShaderModule(const Jasper::SPVShader& shader, Jasper::ShaderStageFlagBits stage);
		~ShaderModule();

		VkPipelineShaderStageCreateInfo CreateInfo();

	private:
		VULKAN_HANDLE(VkShaderModule, m_ShaderModule);
		Jasper::ShaderStageFlagBits m_Stage;
	};
}