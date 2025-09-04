#include "Jasper/Shaders/ShaderCompiler.hpp"

#include "Jasper/Util/Util.hpp"

#include <shaderc/shaderc.hpp>

#include <spirv_cross/spirv_cross.hpp>

namespace Jasper
{
    namespace Util
    {
        namespace
        {
            void ReflectDescriptorSetLayoutBinding(SPVShader& shader, const spirv_cross::Compiler& compiler, const spv::Id& id,
                const DescriptorType descriptorType, const ShaderStageFlagBits stage, const bool debug)
            {
                const uint32_t set = compiler.get_decoration(id, spv::DecorationDescriptorSet);
                const uint32_t binding = compiler.get_decoration(id, spv::DecorationBinding);

                auto& bindingsVector = shader.DescriptorSetLayoutBindings[set];

                // Check if the binding already exists
                if (const auto it = std::ranges::find_if(bindingsVector, [binding, descriptorType](const DescriptorSetLayoutBinding& b)
                    { return b.Binding == binding && b.Type == descriptorType; });
                    it != bindingsVector.end())
                {
                    it->StageFlags |= stage;
                    
                    if (debug) OPAL_CORE_INFO(" [=] Jasper::ShaderCompiler::ReflectSPV() - Updated DescriptorSetLayoutBinding ({}): stage={}, set={}, binding={}",
                        Util::DescriptorTypeToString(it->Type), Util::ShaderStageToString(stage), set, binding);
                }
                else
                {
                    DescriptorSetLayoutBinding layoutBinding;
                    layoutBinding.Binding = binding;
                    layoutBinding.Type = descriptorType;
                    layoutBinding.Count = 1;
                    layoutBinding.StageFlags |= stage;
                    shader.DescriptorSetLayoutBindings[set].push_back(layoutBinding);
                    
                    if (debug) OPAL_CORE_INFO(" [+] Jasper::ShaderCompiler::ReflectSPV() - Found DescriptorSetLayoutBinding ({}): stage={}, set={}, binding={}",
                        Util::DescriptorTypeToString(descriptorType), Util::ShaderStageToString(stage), set, binding);
                }
            }
        }
    }
    
    ShaderCompiler::ShaderCompiler(const Flags& flags)
    {
        m_Flags = flags;
    }

    std::vector<uint32_t> ShaderCompiler::CompileSPV(const std::string& source, const std::filesystem::path& path, const ShaderStageFlagBits stage) const
    {
        const shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        switch (m_Flags.Environment)
        {
            case Vulkan: options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2); break;
            case OpenGL: options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5); break;
            case DirectX:
            case None:
                OPAL_CORE_ERROR("Jasper::ShaderCompiler::CompileSPV() - Target Environment not supported yet!");
                break;
        }
        
        const auto shaderKind = static_cast<shaderc_shader_kind>(Util::ShaderStageToShaderCStage(stage));

        auto preProcessed = compiler.PreprocessGlsl(source, shaderKind, path.filename().string().c_str(), options);
        const auto compiled = compiler.CompileGlslToSpv(source, shaderKind, path.filename().string().c_str(), options);


        if (compiled.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            OPAL_CORE_ERROR(compiled.GetErrorMessage());
            OPAL_CORE_ASSERT(false)
        }

        if (m_Flags.Debug) OPAL_CORE_INFO("Jasper::ShaderCompiler::CompileSPV() - Compiled {} Shader: {}", Util::ShaderStageToString(stage), path.string().c_str());

        return { compiled.cbegin(), compiled.cend() };
    }

    void ShaderCompiler::ReflectSPV(SPVShader& shader, const ShaderStageFlagBits stage) const
    {
        spirv_cross::Compiler stageCompiler(shader.Data.at(stage));
        spirv_cross::ShaderResources stageResources = stageCompiler.get_shader_resources();

        if (stage == Vertex)
        {
            // Sort resources by location
            // this is required to calculate the offsets properly
            std::vector stageInputs(stageResources.stage_inputs.begin(), stageResources.stage_inputs.end());
            std::ranges::sort(stageInputs, [&stageCompiler](const spirv_cross::Resource& a, const spirv_cross::Resource& b)
            {
                const uint32_t locationA = stageCompiler.get_decoration(a.id, spv::DecorationLocation);
                const uint32_t locationB = stageCompiler.get_decoration(b.id, spv::DecorationLocation);
                return locationA < locationB;
            });

            std::unordered_map<uint32_t, uint32_t> bindingStrides;

            for (const spirv_cross::Resource& resource : stageInputs)
            {
                const uint32_t location = stageCompiler.get_decoration(resource.id, spv::DecorationLocation);
                uint32_t binding = stageCompiler.get_decoration(resource.id, spv::DecorationBinding);
                const auto& type = stageCompiler.get_type(resource.base_type_id);

                const Format format = Util::GetFormatFromSPIRType(type);
                const uint32_t size = type.vecsize * 4; // Assuming 4 bytes per component (float, int, uint)

                VertexInputAttributeDescription attributeDescription{};
                attributeDescription.Location = location;
                attributeDescription.Binding = binding;
                attributeDescription.Format = format;
                attributeDescription.Offset = bindingStrides[binding];
                shader.VertexInputAttributes.emplace_back(attributeDescription);
                
                std::string name = stageCompiler.get_name(resource.id);
                if (name.empty()) name = stageCompiler.get_fallback_name(resource.id);
                shader.VertexAttributeNames.push_back(name);

                if (m_Flags.Debug) OPAL_CORE_INFO(" [+] Jasper::ShaderCompiler::ReflectSPV() - Found VertexInputAttribute: layout(location = {}, binding = {}, offset = {}) in {} {};",
                    location, binding, attributeDescription.Offset, Util::FormatToString(format), name);

                bindingStrides[binding] += size;
            }

            for (const auto& [binding, stride] : bindingStrides)
            {
                VertexInputBindingDescription bindingDescription{};
                bindingDescription.Binding = binding;
                bindingDescription.Stride = stride;
                bindingDescription.InputRate = PerVertex;
                shader.VertexInputBindings.emplace_back(bindingDescription);
            }
        }

        // ===== PushConstants =====
        if (auto pushConstantBuffers = stageCompiler.get_shader_resources().push_constant_buffers;
            !pushConstantBuffers.empty())
        {
            shader.PushConstant.StageFlags |= stage;
            for (const auto& pushConstantBuffer : pushConstantBuffers)
            {
                const spirv_cross::SPIRType& bufferType = stageCompiler.get_type(pushConstantBuffer.base_type_id);
                shader.PushConstant.Size = static_cast<uint32_t>(stageCompiler.get_declared_struct_size(bufferType));
                stageCompiler.get_binary_offset_for_decoration(pushConstantBuffer.id, spv::DecorationBinding, shader.PushConstant.Offset);

                if (m_Flags.Debug) OPAL_CORE_INFO(" [+] Jasper::ShaderCompiler::ReflectSPV() - Found PushConstant: stage={}, size={}, offset={}",
                    Util::ShaderStageToString(stage), shader.PushConstant.Size, shader.PushConstant.Offset);
            }
        }

        // ===== Descriptors =====
        for (const spirv_cross::Resource& resource : stageResources.uniform_buffers)
            Util::ReflectDescriptorSetLayoutBinding(shader, stageCompiler, resource.id, UniformBuffer, stage, m_Flags.Debug);
        
        for (const spirv_cross::Resource& resource : stageResources.sampled_images)
            Util::ReflectDescriptorSetLayoutBinding(shader, stageCompiler, resource.id, CombinedImageSampler, stage, m_Flags.Debug);
        
        for (const spirv_cross::Resource& resource : stageResources.storage_buffers)
            Util::ReflectDescriptorSetLayoutBinding(shader, stageCompiler, resource.id, StorageBuffer, stage, m_Flags.Debug);
    }
}
