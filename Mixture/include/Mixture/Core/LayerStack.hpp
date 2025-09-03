#pragma once

#include "Mixture/Core/Layer.hpp"

namespace Mixture
{
    class LayerStack
    {
    public:
        OPAL_NON_COPIABLE(LayerStack);
        
        LayerStack() = default;
        ~LayerStack() = default;

        void Shutdown() const;

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* layer);

        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }
        std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
        std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

        OPAL_NODISCARD std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
        OPAL_NODISCARD std::vector<Layer*>::const_iterator end() const { return m_Layers.end(); }
        OPAL_NODISCARD std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
        OPAL_NODISCARD std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }

    private:
        std::vector<Layer*> m_Layers;
        unsigned int m_LayerInsertIndex = 0;
    };
}
