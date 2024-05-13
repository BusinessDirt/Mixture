#pragma once

#include "Entity.h"

namespace Mixture {
	class ScriptableEntity {
	public:
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T& getComponent() {
			return m_Entity.getComponent<T>();
		}
	protected:
		virtual void onCreate() {}
		virtual void onDestroy() {}
		virtual void onUpdate(Timestep ts) {}
	private:
		Entity m_Entity;
		friend class Scene;
	};
}