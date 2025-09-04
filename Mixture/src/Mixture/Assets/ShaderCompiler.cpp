#include "mxpch.hpp"
#include "Mixture/Assets/ShaderCompiler.hpp"

#include "Platform/Vulkan/Base.hpp"

#include "Mixture/Util/Util.hpp"
#include "Mixture/Util/ToString.hpp"

#include "Mixture/Core/Application.hpp"

#include <algorithm>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>

namespace Mixture
{
    namespace Util
    {
        namespace
        {
            shaderc_shader_kind ShaderStageToShaderC(const ShaderStage stage)
            {
                if (stage == ShaderStageVertex) return shaderc_vertex_shader;
                if (stage == ShaderStageFragment) return shaderc_fragment_shader;

                OPAL_CORE_ERROR("Unsupported shader type");
                return shaderc_vertex_shader; // these issues should be caught at runtime so returning this shouldn't be a problem
            }

            VkFormat GetVkFormat(const spirv_cross::SPIRType& type)
            {
                if (type.basetype == spirv_cross::SPIRType::Float)
                {
                    if (type.vecsize == 1) return VK_FORMAT_R32_SFLOAT;
                    if (type.vecsize == 2) return VK_FORMAT_R32G32_SFLOAT;
                    if (type.vecsize == 3) return VK_FORMAT_R32G32B32_SFLOAT;
                    if (type.vecsize == 4) return VK_FORMAT_R32G32B32A32_SFLOAT;
                }
                if (type.basetype == spirv_cross::SPIRType::Int)
                {
                    if (type.vecsize == 1) return VK_FORMAT_R32_SINT;
                    if (type.vecsize == 2) return VK_FORMAT_R32G32_SINT;
                    if (type.vecsize == 3) return VK_FORMAT_R32G32B32_SINT;
                    if (type.vecsize == 4) return VK_FORMAT_R32G32B32A32_SINT;
                }
                if (type.basetype == spirv_cross::SPIRType::UInt)
                {
                    if (type.vecsize == 1) return VK_FORMAT_R32_UINT;
                    if (type.vecsize == 2) return VK_FORMAT_R32G32_UINT;
                    if (type.vecsize == 3) return VK_FORMAT_R32G32B32_UINT;
                    if (type.vecsize == 4) return VK_FORMAT_R32G32B32A32_UINT;
                }

                OPAL_CORE_ERROR("Unsupported Format");
                return VK_FORMAT_UNDEFINED;
            }
        
            void ReflectDescriptorSetLayoutBinding(SpvShader& shader, const spirv_cross::Compiler& compiler, const spv::Id& id, const VkDescriptorType descriptorType)
            {
                const uint32_t set = compiler.get_decoration(id, spv::DecorationDescriptorSet);
                const uint32_t binding = compiler.get_decoration(id, spv::DecorationBinding);
                
                VkDescriptorSetLayoutBinding layoutBinding;
                layoutBinding.binding            = binding;
                layoutBinding.descriptorType     = descriptorType;
                layoutBinding.descriptorCount    = 1;
                layoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                layoutBinding.pImmutableSamplers = nullptr;
                shader.DescriptorSetLayoutBindings[set].push_back(layoutBinding);
            }
        }
    }
    
	namespace ShaderCompiler
	{
		SpvShader Compile(const std::string& shaderName, const Flags& compileFlags)
		{
            const std::filesystem::path shaderPath = Application::Get().GetAssetManager().GetShaderPath();
            SpvShader spvShader{};

            // Get a list of files for the given compiler stage flags
            auto shaderFiles = Vector<std::filesystem::path>();
            switch (compileFlags.PipelineType)
            {
                case Graphics:
                {
                    // .vert.glsl and .frag.glsl
                    shaderFiles.push_back(shaderPath / (shaderName + ".vert.glsl"));
                    shaderFiles.push_back(shaderPath / (shaderName + ".frag.glsl"));
                }
            }

            // compile every stage
            for (const auto& shaderFile : shaderFiles)
            {
                ShaderStage stage = Util::FilePathToShaderStage(shaderFile);
                spvShader.Data[stage] = CompileStage(shaderFile, compileFlags, stage);
            }

            Reflect(spvShader);
            if (compileFlags.Debug) DebugPrint(shaderName, compileFlags, spvShader);

            return spvShader;
		}

		void Reflect(SpvShader& shader)
		{
            spirv_cross::Compiler vertCompiler(shader.Data.at(ShaderStageVertex));
            spirv_cross::ShaderResources vertResources = vertCompiler.get_shader_resources();

            // ==== Push Constant ====
            shader.PushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            for (const spirv_cross::Resource& pushConstant : vertResources.push_constant_buffers)
            {
                const spirv_cross::SPIRType& bufferType = vertCompiler.get_type(pushConstant.base_type_id);
                shader.PushConstant.size = static_cast<uint32_t>(vertCompiler.get_declared_struct_size(bufferType));
                vertCompiler.get_binary_offset_for_decoration(pushConstant.id, spv::DecorationBinding, shader.PushConstant.offset);
            }
            
            // ==== Descriptor Set Layout Binding ====
            for (const spirv_cross::Resource& resource : vertResources.uniform_buffers)
                Util::ReflectDescriptorSetLayoutBinding(shader, vertCompiler, resource.id, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            
            for (const spirv_cross::Resource& resource : vertResources.sampled_images)
                Util::ReflectDescriptorSetLayoutBinding(shader, vertCompiler, resource.id, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            
            for (const spirv_cross::Resource& resource : vertResources.storage_buffers)
                Util::ReflectDescriptorSetLayoutBinding(shader, vertCompiler, resource.id, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
            

            // ==== Vertex Attributes and Bindings ====
            // Sort resources by location
            // this is required to calculate the offsets properly
            std::vector stageInputs(vertResources.stage_inputs.begin(), vertResources.stage_inputs.end());
            std::ranges::sort(stageInputs, [&vertCompiler](const spirv_cross::Resource& a, const spirv_cross::Resource& b)
            {
                const uint32_t locationA = vertCompiler.get_decoration(a.id, spv::DecorationLocation);
                const uint32_t locationB = vertCompiler.get_decoration(b.id, spv::DecorationLocation);
                return locationA < locationB;
            });

            std::unordered_map<uint32_t, uint32_t> bindingStrides;

            for (const spirv_cross::Resource& resource : stageInputs)
            {
                const uint32_t location = vertCompiler.get_decoration(resource.id, spv::DecorationLocation);
                uint32_t binding = vertCompiler.get_decoration(resource.id, spv::DecorationBinding);
                const auto& type = vertCompiler.get_type(resource.base_type_id);

                const VkFormat format = Util::GetVkFormat(type);
                const uint32_t size = type.vecsize * 4; // Assuming 4 bytes per component (float, int, uint)

                VkVertexInputAttributeDescription attributeDescription{};
                attributeDescription.location = location;
                attributeDescription.binding = binding;
                attributeDescription.format = format;
                attributeDescription.offset = bindingStrides[binding];
                shader.VertexInputAttributes.emplace_back(attributeDescription);
                
                std::string name = vertCompiler.get_name(resource.id);
                if (name.empty()) name = vertCompiler.get_fallback_name(resource.id);
                shader.VertexAttributeNames.push_back(name);

                bindingStrides[binding] += size;
            }

            for (const auto& [binding, stride] : bindingStrides)
            {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = binding;
                bindingDescription.stride = stride;
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                shader.VertexInputBindings.emplace_back(bindingDescription);
            }
		}

        void DebugPrint(const std::string& shaderName, const Flags& compileFlags, SpvShader& shader)
        {
            VULKAN_INFO_BEGIN("Compiled Shader");
            // General Information
            VULKAN_INFO_LIST("Name: {}", 0, shaderName);
            VULKAN_INFO_LIST("Pipeline Type: {}", 0, compileFlags.PipelineType == Graphics ? "Graphics" : "Unknown");

            // Push Constant
            if (shader.PushConstant.size > 0)
            {
                VULKAN_INFO_LIST_HEADER("Push Constant", 0);
                VULKAN_INFO_LIST("Size: {}", 1, shader.PushConstant.size);
                VULKAN_INFO_LIST("Offset: {}", 1, shader.PushConstant.offset);
            }
            
            // Descriptor Set Layout Bindings
            if (!shader.DescriptorSetLayoutBindings.empty())
            {
                VULKAN_INFO_LIST_HEADER("Descriptor Set Layout Bindings", 0);
                
                for (const auto& [setIndex, bindings] : shader.DescriptorSetLayoutBindings)
                    {
                        for (const auto& binding : bindings)
                        {
                            VULKAN_INFO_LIST("set = {}, binding = {}, type = {}, count = {}", 1, setIndex, binding.binding,
                                Vulkan::ToString::DescriptorType(binding.descriptorType), binding.descriptorCount);
                        }
                    }
            }

            // Vertex Input Bindings
            if (!shader.VertexInputBindings.empty())
            {
                VULKAN_INFO_LIST_HEADER("Vertex Input Bindings", 0);
                
                for (const auto& binding : shader.VertexInputBindings)
                {
                    VULKAN_INFO_LIST("Binding {}:", 1, binding.binding);
                    VULKAN_INFO_LIST("Stride: {}", 2, binding.stride);
                }
            }

            // Vertex Input Attributes
            if (!shader.VertexInputAttributes.empty())
            {
                VULKAN_INFO_LIST_HEADER("Vertex Attributes", 0);

                int i = 0;
                for (const auto& [location, binding, format, offset] : shader.VertexInputAttributes)
                {
                    VULKAN_INFO_LIST("layout(location = {}, binding = {}, offset = {}) in {} {};", 1, location, binding, offset, Vulkan::ToString::ShaderFormat(format), shader.VertexAttributeNames[i]);
                    i++;
                }
            }

            VULKAN_INFO_END();
        }

		std::vector<uint32_t> CompileStage(const std::filesystem::path& path, const Flags& compileFlags, const ShaderStage stage)
		{
		    const shaderc::Compiler compiler;
		    shaderc::CompileOptions options;
		    const shaderc_shader_kind shaderKind = Util::ShaderStageToShaderC(stage);

		    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

		    const std::string source = Util::ReadFile(path);

		    auto preProcessed = compiler.PreprocessGlsl(source, shaderKind, path.filename().string().c_str(), options);
		    const auto compiled = compiler.CompileGlslToSpv(source, shaderKind, path.filename().string().c_str(), options);


		    if (compiled.GetCompilationStatus() != shaderc_compilation_status_success)
		    {
		        OPAL_CORE_ERROR(compiled.GetErrorMessage());
		        OPAL_CORE_ASSERT(false)
            }

		    return { compiled.cbegin(), compiled.cend() };
		}
	}
}
