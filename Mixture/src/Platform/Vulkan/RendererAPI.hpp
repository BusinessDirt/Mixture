#pragma once

#include "Mixture/Renderer/RendererAPI.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{

    class RendererAPI : public ::Mixture::RendererAPI
    {
    public:
        ~RendererAPI() override;

        void Init(const std::string& applicationName) override;

        void OnWindowResize(uint32_t width, uint32_t height) override;

        bool BeginFrame() override;
        bool EndFrame() override;

    private:
        Context* m_Context;
    };
}
