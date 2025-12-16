#pragma once

#include "Mixture/Events/Event.hpp"

namespace Mixture
{
    class RenderGraph;

    /**
     * @brief Abstract base class for application layers.
     * 
     * Layers are pushed to the LayerStack and updated/rendered in order.
     */
    class Layer
    {
    public:
        OPAL_NON_COPIABLE(Layer);
        
        /**
         * @brief Constructor.
         * 
         * @param name Debug name of the layer.
         */
        explicit Layer(std::string name = "Layer") : m_DebugName(std::move(name)) {}
        virtual ~Layer() = default;

        /**
         * @brief Called when the layer is attached to the layer stack.
         */
        virtual void OnAttach() {};

        /**
         * @brief Called when the layer is detached from the layer stack.
         */
        virtual void OnDetach() {};
        
        /**
         * @brief Called when an event is dispatched to this layer.
         * 
         * @param event The event to handle.
         */
        virtual void OnEvent(Event& event) {};

        virtual void OnUpdate(float dt) {};

        virtual void OnRender(RenderGraph& graph) {};

        /**
         * @brief Gets the debug name of the layer.
         * 
         * @return const std::string& The name.
         */
        OPAL_NODISCARD const std::string& GetName() const { return m_DebugName; }

    protected:
        std::string m_DebugName;
    };
}
