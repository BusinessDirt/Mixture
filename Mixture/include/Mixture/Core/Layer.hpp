#pragma once

#include "Mixture/Events/Event.hpp"

namespace Mixture
{
    class Layer
    {
    public:
        OPAL_NON_COPIABLE(Layer);
        
        explicit Layer(std::string name = "Layer") : m_DebugName(std::move(name)) {}
        virtual ~Layer() = default;

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        
        virtual void OnEvent(Event& event) = 0;

        OPAL_NODISCARD const std::string& GetName() const { return m_DebugName; }

    protected:
        std::string m_DebugName;
    };
}
