#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/Shaders/ShaderInformation.hpp"

#include <filesystem>

namespace Mixture
{
	enum PipelineType
	{
		GRAPHICS_PIPELINE
	};

	class ShaderCompiler
	{
	public:
		struct Flags
		{
			PipelineType PipelineType = GRAPHICS_PIPELINE;
		};

	public:
		static SPVShader Compile(const std::string& shaderName, const Flags& compileFlags);
		static void Reflect(SPVShader& spvShader);

	private:
		static Vector<uint32_t> CompileStage(const std::filesystem::path& path, const Flags& compileFlags, ShaderStage stage);
	};
}


