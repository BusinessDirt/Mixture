#include "mxpch.h"
#include "Entity.h"

namespace Mixture {
	Entity::Entity(entt::entity handle, Scene* scene) 
		: m_EntityHandle(handle), m_Scene(scene) {
	}
}

