#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Events/Event.hpp"
#include "Mixture/Renderer/FrameInfo.hpp"

namespace Mixture
{
    class Layer
    {
    public:
        Layer(const std::string& name = "Layer") : m_DebugName(name) {}
        virtual ~Layer() = default;

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate(FrameInfo& frameInfo) = 0;
        virtual void OnUIRender() = 0;
        virtual void OnEvent(Event& event) = 0;

        const std::string& GetName() const { return m_DebugName; }

    protected:
        std::string m_DebugName;
    };
}
