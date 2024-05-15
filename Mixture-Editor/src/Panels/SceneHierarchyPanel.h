#pragma once

#include "Mixture/Core/Base.h"
#include "Mixture/Scene/Scene.h"
#include "Mixture/Scene/Entity.h"

namespace Mixture {
	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void setContext(const Ref<Scene>& context);

		void onImGuiRender();

		Entity getSelectedEntity() const { return m_SelectionContext; }
	private:
		void drawEntityNode(Entity entity);
		void drawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}
