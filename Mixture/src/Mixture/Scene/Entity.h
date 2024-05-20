#pragma once

#include "Mixture/Core/UUID.h"
#include "Mixture/Scene/Scene.h"
#include "Mixture/Scene/Components.h"

#include "entt.hpp"

namespace Mixture {
	class Entity {
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& addComponent(Args&&... args) {
			MX_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->onComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& addOrReplaceComponent(Args&&... args) {
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->onComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& getComponent() {
			MX_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool hasComponent() {
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template<typename T>
		void removeComponent() {
			MX_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		UUID getUUID() { return getComponent<IDComponent>().id; }
		const std::string& getName() { return getComponent<TagComponent>().tag; }

		bool operator==(const Entity& other) const {
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const {
			return !(*this == other);
		}
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};
}
