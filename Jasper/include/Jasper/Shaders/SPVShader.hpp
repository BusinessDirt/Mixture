#pragma once

#include <Opal/Base.hpp>

#include "Jasper/Shaders/Types/Format.hpp"
#include "Jasper/Shaders/Types/DescriptorType.hpp"
#include "Jasper/Shaders/Types/VertexInputRate.hpp"
#include "Jasper/Shaders/Types/ShaderStageFlagBits.hpp"

namespace Jasper
{
	typedef uint32_t ShaderStageFlags;
	struct PushConstantRange
	{
		ShaderStageFlags StageFlags;
		uint32_t Offset;
		uint32_t Size;
	};

	struct DescriptorSetLayoutBinding
	{
		uint32_t Binding = 0;
		DescriptorType Type;
		uint32_t Count = 0;
		ShaderStageFlags StageFlags = 0;
		//const VkSampler* pImmutableSamplers;
	};

	struct VertexInputBindingDescription
	{
		uint32_t Binding;
		uint32_t Stride;
		VertexInputRate InputRate;
	};

	struct VertexInputAttributeDescription
	{
		uint32_t Location;
		uint32_t Binding;
		Format Format;
		uint32_t Offset;
	};

	struct SPVShader
	{
		std::string Name;
		std::unordered_map<ShaderStageFlagBits, std::vector<uint32_t>> Data;
		
		std::vector<VertexInputBindingDescription> VertexInputBindings;
		std::vector<VertexInputAttributeDescription> VertexInputAttributes;
		std::vector<std::string> VertexAttributeNames;
		
		PushConstantRange PushConstant{};
		
        std::unordered_map<uint32_t, std::vector<DescriptorSetLayoutBinding>> DescriptorSetLayoutBindings;
	};

	namespace Util
	{
		void PrintSPVShaderDebugInformation(SPVShader& shader);
		size_t HashSPVShader(const SPVShader& shader);
	}
}
