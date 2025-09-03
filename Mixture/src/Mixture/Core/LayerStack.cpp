#include "mxpch.hpp"
#include "Mixture/Core/LayerStack.hpp"

namespace Mixture
{
    void LayerStack::Shutdown() const
    {
        for (Layer* layer : m_Layers)
        {
            layer->OnDetach();
            delete layer;
        }
    }

    void LayerStack::PushLayer(Layer* layer)
    {
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
        layer->OnAttach();
    }

    void LayerStack::PushOverlay(Layer* layer)
    {
        m_Layers.emplace_back(layer);
        layer->OnAttach();
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        if (const auto it = std::ranges::find(m_Layers, layer);
            it != m_Layers.begin() + m_LayerInsertIndex)
        {
            layer->OnDetach();
            m_Layers.erase(it);
            m_LayerInsertIndex--;
        }
    }

    void LayerStack::PopOverlay(Layer* layer)
    {
        if (const auto it = std::ranges::find(m_Layers, layer);
            it != m_Layers.end())
        {
            layer->OnDetach();
            m_Layers.erase(it);
        }
    }
}
