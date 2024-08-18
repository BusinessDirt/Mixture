#include "mxpch.hpp"
#include "AssetManager.hpp"

namespace Mixture
{
    namespace Util
    {
        std::filesystem::path GetProjectRootPath()
        {
            std::filesystem::path currentPath = std::filesystem::current_path();
            while (!currentPath.empty())
            {
                if (std::filesystem::exists(currentPath / ".git")) return currentPath;
                currentPath = currentPath.parent_path();
            }

            return {};
        }
    }
    AssetManager::AssetManager()
    {
        // TODO: replace "App" with a macro or similiar
        std::filesystem::path relativePath = Util::GetProjectRootPath() / "App" / "assets";
        m_AssetsPath = std::filesystem::absolute(relativePath);
        
        m_ShaderManager = CreateScope<ShaderManager>(m_AssetsPath / "shaders");
    }

    AssetManager::~AssetManager()
    {
        m_ShaderManager = nullptr;
    }

    const ShaderCode& AssetManager::GetShader(const std::string& filename) const
    {
        return m_ShaderManager->m_Shaders.at(filename);
    }

    std::filesystem::path AssetManager::GetTexturePath(const std::string& filename) const
    {
        return m_AssetsPath / "textures" / filename;
    }

    std::vector<Vulkan::DescriptorBinding> AssetManager::GetDescriptorBindings() const
    {
        std::vector<Vulkan::DescriptorBinding> bindings = std::vector<Vulkan::DescriptorBinding>();
        
        // ubos
        for (const auto& ubo : m_ShaderManager->m_UniformBufferInfos)
        {
            Vulkan::DescriptorBinding binding{};
            binding.Binding = ubo.Binding;
            binding.DescriptorCount = 1;
            binding.Stage = ubo.Flags;
            binding.Type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings.emplace_back(binding);
        }
        
        // combined image samplers
        for (const auto& image : m_ShaderManager->m_SampledImages)
        {
            Vulkan::DescriptorBinding binding{};
            binding.Binding = image.Binding;
            binding.DescriptorCount = 1;
            binding.Stage = image.Flags;
            binding.Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            bindings.emplace_back(binding);
        }
        
        return bindings;
    }
}
