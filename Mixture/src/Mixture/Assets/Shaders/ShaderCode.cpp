#include "mxpch.hpp"
#include "ShaderCode.hpp"

#include "Mixture/Util/Util.hpp"


namespace Mixture
{
    namespace Util
    {
        static std::string ReadFile(const std::filesystem::path& filepath) 
        {
            std::string result;
            std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
            
            if (in)
            {
                in.seekg(0, std::ios::end);
                size_t size = in.tellg();
                
                if (size != -1)
                {
                    result.resize(size);
                    in.seekg(0, std::ios::beg);
                    in.read(&result[0], size);
                }
                else MX_CORE_ERROR("Could not read from file '{0}'", filepath.string().c_str());

            }
            else MX_CORE_ERROR("Could not open file '{0}'", filepath.string().c_str());

            return result;
        }

        static shaderc_shader_kind ShaderStageToShaderC(const std::filesystem::path& path)
        {
            std::string filename = path.filename().string();
            if (Contains(filename, "vert"))
            {
                return shaderc_glsl_vertex_shader;
            }
            else if (Contains(filename, "frag"))
            {
                return shaderc_glsl_fragment_shader;
            }

            MX_CORE_ERROR("Unsupported shader type");
            return shaderc_vertex_shader; // these issues should be catched at runtime so returning this shouldn't be a problem
        }

        static std::string ShaderStageToString(const std::filesystem::path& path)
        {
            std::string filename = path.filename().string();
            if (Contains(filename, "vert"))
            {
                return "GLSL_VERTEX_SHADER";
            }
            else if (Contains(filename, "frag"))
            {
                return "GLSL_FRAGMENT_SHADER";
            }
            
            MX_CORE_ERROR("Unsupported shader type");
            return "";
        }
    
        static VkShaderStageFlagBits ShaderStageToFlagBits(const std::filesystem::path& path)
        {
            std::string filename = path.filename().string();
            if (Contains(filename, "vert"))
            {
                return VK_SHADER_STAGE_VERTEX_BIT;
            }
            else if (Contains(filename, "frag"))
            {
                return VK_SHADER_STAGE_FRAGMENT_BIT;
            }
            
            MX_CORE_ERROR("Unsupported shader type");
            return VK_SHADER_STAGE_ALL;
        }

        static VkFormat GetVkFormat(const spirv_cross::SPIRType& type)
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

            MX_CORE_ERROR("Unsupported Format");
            return VK_FORMAT_UNDEFINED;
        }
    }

    ShaderCode::ShaderCode(const std::filesystem::path& path, const std::filesystem::path& cachePath,
        std::unordered_set<UniformBufferInformation>& uboInfos, bool compile)
    {
        if (compile)
        {
            Compile(path, cachePath);
        }
        else
        {
            std::ifstream in(cachePath, std::ios::in | std::ios::binary);
            if (in.is_open())
            {
                in.seekg(0, std::ios::end);
                std::streampos size = in.tellg();
                in.seekg(0, std::ios::beg);

                std::vector<uint32_t>& data = m_Data;
                data.resize(size / sizeof(uint32_t));
                in.read((char*)data.data(), size);
            }
        }
        
        m_ShaderStageFlagBits = Util::ShaderStageToFlagBits(path);
        Reflect(path, uboInfos);
    }

    VkShaderModuleCreateInfo ShaderCode::CreateInfo() const
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = m_Data.size() * 4;
        createInfo.pCode = reinterpret_cast<const uint32_t*>(m_Data.data());
        return createInfo;
    }

    void ShaderCode::Compile(const std::filesystem::path& path, const std::filesystem::path& cachePath)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        shaderc_shader_kind shaderKind = Util::ShaderStageToShaderC(path);

        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

        std::string source = Util::ReadFile(path);

        auto preProcessed = compiler.PreprocessGlsl(source, shaderKind, path.filename().string().c_str(), options);
        auto compiled = compiler.CompileGlslToSpv(source, shaderKind, path.filename().string().c_str(), options);


        if (compiled.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            MX_CORE_ERROR(compiled.GetErrorMessage());
            MX_CORE_ASSERT(false);
        }

        m_Data = std::vector<uint32_t>(compiled.cbegin(), compiled.cend());

        std::ofstream out(cachePath, std::ios::out | std::ios::binary);
        if (out.is_open())
        {
            std::vector<uint32_t>& data = m_Data;
            out.write((char*)data.data(), data.size() * sizeof(uint32_t));
            out.flush();
            out.close();
        }
    }

    void ShaderCode::Reflect(const std::filesystem::path& path, std::unordered_set<UniformBufferInformation>& uboInfos)
    {
        //MX_CORE_INFO("Reflecting on shader: {0}", path.filename().string().c_str());

        spirv_cross::Compiler compiler(m_Data);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        // Uniform buffers
        for (const spirv_cross::Resource& resource : resources.uniform_buffers)
        {
            const spirv_cross::SPIRType& bufferType = compiler.get_type(resource.base_type_id);

            UniformBufferInformation bufferInfo{};
            bufferInfo.Size = static_cast<uint32_t>(compiler.get_declared_struct_size(bufferType));
            bufferInfo.Binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            compiler.get_binary_offset_for_decoration(resource.id, spv::DecorationBinding, bufferInfo.Offset);
            
            // Insert the bufferInfo into the set and get the pointer to the actual stored element
            auto [it, inserted] = uboInfos.insert(bufferInfo);
            m_ShaderInformation.UniformBuffers.push_back(&(*it));
        }

        // Push Constants
        for (const spirv_cross::Resource& resource : resources.push_constant_buffers)
        {
            const spirv_cross::SPIRType& bufferType = compiler.get_type(resource.base_type_id);
            m_ShaderInformation.PushConstantInformation.Size = static_cast<uint32_t>(compiler.get_declared_struct_size(bufferType));
            compiler.get_binary_offset_for_decoration(resource.id, spv::DecorationBinding, m_ShaderInformation.PushConstantInformation.Offset);
        }

        // Sort resources by location
        // this is required to calculate the offsets properly
        std::vector<spirv_cross::Resource> stageInputs(resources.stage_inputs.begin(), resources.stage_inputs.end());
        std::sort(stageInputs.begin(), stageInputs.end(), [&compiler](const spirv_cross::Resource& a, const spirv_cross::Resource& b)
            {
                uint32_t locationA = compiler.get_decoration(a.id, spv::DecorationLocation);
                uint32_t locationB = compiler.get_decoration(b.id, spv::DecorationLocation);
                return locationA < locationB;
            });

        std::unordered_map<uint32_t, uint32_t> bindingStrides;

        for (const spirv_cross::Resource& resource : stageInputs)
        {
            uint32_t location = compiler.get_decoration(resource.id, spv::DecorationLocation);
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            auto type = compiler.get_type(resource.base_type_id);

            VkFormat format = Util::GetVkFormat(type);
            uint32_t size = type.vecsize * 4; // Assuming 4 bytes per component (float, int, uint)
            
            VkVertexInputAttributeDescription attributeDescription{};
            attributeDescription.location = location;
            attributeDescription.binding = binding;
            attributeDescription.format = format;
            attributeDescription.offset = bindingStrides[binding];
            m_ShaderInformation.Attributes.emplace_back(attributeDescription);
            
            bindingStrides[binding] += size;
        }

        for (const auto& [binding, stride] : bindingStrides)
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = binding;
            bindingDescription.stride = stride;
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Default value
            m_ShaderInformation.Bindings.emplace_back(bindingDescription);
        }
    }
}
