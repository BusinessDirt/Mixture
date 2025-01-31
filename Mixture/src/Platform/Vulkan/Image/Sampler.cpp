#include "mxpch.hpp"
#include "Sampler.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    Sampler::Sampler(const SamplerConfig& config)
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = config.MagFilter;
        samplerInfo.minFilter = config.MinFilter;
        samplerInfo.addressModeU = config.AddressModeU;
        samplerInfo.addressModeV = config.AddressModeV;
        samplerInfo.addressModeW = config.AddressModeW;
        samplerInfo.anisotropyEnable = config.AnisotropyEnable;
        samplerInfo.maxAnisotropy = Context::Get().GetPhysicalDevice().GetProperties().limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = config.BorderColor;
        samplerInfo.unnormalizedCoordinates = config.UnnormalizedCoordinates;
        samplerInfo.compareEnable = config.CompareEnable;
        samplerInfo.compareOp = config.CompareOp;
        samplerInfo.mipmapMode = config.MipmapMode;
        samplerInfo.mipLodBias = config.MipLodBias;
        samplerInfo.minLod = config.MinLod;
        samplerInfo.maxLod = config.MaxLod;

        MX_VK_ASSERT(vkCreateSampler(Context::Get().GetDevice().GetHandle(), &samplerInfo, nullptr, &m_Sampler),
            "Failed to create VkSampler");
    }

    Sampler::~Sampler()
    {
        if (m_Sampler)
        {
            vkDestroySampler(Context::Get().GetDevice().GetHandle(), m_Sampler, nullptr);
            m_Sampler = nullptr;
        }
    }
}
