#pragma once

#include "Mixture.hpp"

namespace Mixture
{
    struct UniformBufferObject
    {
        glm::mat4 View;
        glm::mat4 Projection;
    };

    class MainLayer final : public Layer
    {
    public:
        OPAL_NON_COPIABLE(MainLayer);
        MainLayer() : Layer("MainLayer") {}
        ~MainLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        
        void OnEvent(Event& event) override;
    };
}

