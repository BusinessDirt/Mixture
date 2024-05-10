#pragma once

#include "Mixture/Core/Layer.h"

#include "Mixture/Events/ApplicationEvent.h"
#include "Mixture/Events/KeyEvent.h"
#include "Mixture/Events/MouseEvent.h"

namespace Mixture {
	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onImGuiRender() override;

		void begin();
		void end();
	private:
		float m_Time = 0.0f;
	};
}