#include "mxpch.hpp"
#include "LayerStack.hpp"

namespace Mixture
{
    LayerStack::~LayerStack()
    {
        for (Layer* layer : m_Layers)
        {
            layer->OnDetach();
            delete layer;
        }
    }

    void LayerStack::Update(FrameInfo& frameInfo) const
    {
        for (Layer* layer : m_Layers)
            layer->OnUpdate(frameInfo);
    }

    void LayerStack::RenderUI() const
    {
        for (Layer* layer : m_Layers)
            layer->OnUIRender();
    }

    void LayerStack::PushLayer(Layer* layer)
    {
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
        layer->OnAttach();
    }

    void LayerStack::PushOverlay(Layer* overlay)
    {
        m_Layers.emplace_back(overlay);
        overlay->OnAttach();
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        std::vector<Layer*>::iterator it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
        if (it != m_Layers.begin() + m_LayerInsertIndex)
        {
            layer->OnDetach();
            m_Layers.erase(it);
            m_LayerInsertIndex--;
        }
    }

    void LayerStack::PopOverlay(Layer* overlay)
    {
        std::vector<Layer*>::iterator it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
        if (it != m_Layers.end())
        {
            overlay->OnDetach();
            m_Layers.erase(it);
        }
    }
}
