#pragma once

#include "entt.hpp"

#include "Mixture/Core/Base.h"
#include "Mixture/Core/Timestep.h"
#include "Mixture/Core/UUID.h"
#include "Mixture/Renderer/EditorCamera.h"

class b2World;

namespace Mixture {

	class Entity;

	class Scene {
	public:
		Scene();
		~Scene();

		static Ref<Scene> copy(Ref<Scene> other);

		Entity createEntity(const std::string& name = std::string());
		Entity createEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void destroyEntity(Entity entity);

		void onRuntimeStart();
		void onRuntimeStop();

		void onSimulationStart();
		void onSimulationStop();

		// TEMP
		entt::registry& reg() { return m_Registry; }

		void onUpdateRuntime(Timestep ts);
		void onUpdateSimulation(Timestep ts, EditorCamera& camera);
		void onUpdateEditor(Timestep ts, EditorCamera& camera);
		void onViewportResize(uint32_t width, uint32_t height);

		void duplicateEntity(Entity entity);

		Entity getPrimaryCameraEntity();

		template<typename... Components>
		auto getAllEntitiesWith() {
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void onComponentAdded(Entity entity, T& component);

		void onPhysics2DStart();
		void onPhysics2DStop();

		void renderScene(EditorCamera& camera);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}
