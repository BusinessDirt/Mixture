#include "mxpch.hpp"
#include "VulkanGraphicsPipeline.hpp"

#include "Mixture/Core/Application.hpp"

#include "Platform/Vulkan/VulkanContext.hpp"
#include "Platform/Vulkan/VulkanShaderModule.hpp"

namespace Mixture
{

    VulkanGraphicsPipeline::VulkanGraphicsPipeline()
    {
        VulkanShaderModule vertModule(Application::Get().GetAssetManager().GetShader("shader.vert"));
        VulkanShaderModule fragModule(Application::Get().GetAssetManager().GetShader("shader.frag"));
        
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertModule.GetHandle();
        vertShaderStageInfo.pName = "main";
        
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragModule.GetHandle();
        fragShaderStageInfo.pName = "main";
        
        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
    {
        
    }
}
