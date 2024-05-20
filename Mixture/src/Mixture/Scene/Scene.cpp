#include "mxpch.h"
#include "Scene.h"

#include "Mixture/Scene/Components.h"
#include "Mixture/Scene/ScriptableEntity.h"
#include "Mixture/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Entity.h"

// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace Mixture {

	static b2BodyType rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType) {
		switch (bodyType) {
			case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		MX_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	Scene::Scene() {
		
	}

	Scene::~Scene() {

	}

	template<typename Component>
	static void copyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap) {
		auto view = src.view<Component>();
		for (entt::entity e : view) {
			UUID uuid = src.get<IDComponent>(e).id;
			MX_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
			entt::entity dstEnttID = enttMap.at(uuid);

			Component& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEnttID, component);
		}
	}

	template<typename Component>
	static void copyComponentIfExists(Entity dst, Entity src) {
		if (src.hasComponent<Component>())
			dst.addOrReplaceComponent<Component>(src.getComponent<Component>());
	}

	Ref<Scene> Scene::copy(Ref<Scene> other) {
		Ref<Scene> newScene = createRef<Scene>();
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		entt::registry& srcSceneRegistry = other->m_Registry;
		entt::registry& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (entt::entity e : idView) {
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).id;
			const std::string& name = srcSceneRegistry.get<TagComponent>(e).tag;
			Entity newEntity = newScene->createEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components
		copyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		copyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::createEntity(const std::string& name) {
		return createEntityWithUUID(UUID(), name);
	}

	Entity Scene::createEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.addComponent<IDComponent>(uuid);
		entity.addComponent<TransformComponent>();
		auto& tag = entity.addComponent<TagComponent>();
		tag.tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::destroyEntity(Entity entity) {
		m_Registry.destroy(entity);
	}

	void Scene::onRuntimeStart() {
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view) {
			Entity entity = { e, this };
			TransformComponent& transform = entity.getComponent<TransformComponent>();
			Rigidbody2DComponent& rb2d = entity.getComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = rigidbody2DTypeToBox2DBody(rb2d.type);
			bodyDef.position.Set(transform.translation.x, transform.translation.y);
			bodyDef.angle = transform.rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.fixedRotation);
			rb2d.runtimeBody = body;

			if (entity.hasComponent<BoxCollider2DComponent>()) {
				BoxCollider2DComponent& bc2d = entity.getComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.size.x * transform.scale.x, bc2d.size.y * transform.scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.density;
				fixtureDef.friction = bc2d.friction;
				fixtureDef.restitution = bc2d.restitution;
				fixtureDef.restitutionThreshold = bc2d.restitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.hasComponent<CircleCollider2DComponent>()) {
				CircleCollider2DComponent& cc2d = entity.getComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.offset.x, cc2d.offset.y);
				circleShape.m_radius = transform.scale.x * cc2d.radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.density;
				fixtureDef.friction = cc2d.friction;
				fixtureDef.restitution = cc2d.restitution;
				fixtureDef.restitutionThreshold = cc2d.restitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::onRuntimeStop() {
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::onUpdateRuntime(Timestep ts) {

		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
				if (!nsc.instance) {
					nsc.instance = nsc.instantiateScript();
					nsc.instance->m_Entity = Entity{ entity, this };
					nsc.instance->onCreate();
				}

				nsc.instance->onUpdate(ts);
			});
		}

		// Physics
		{
			MX_PROFILE_SCOPE("Scene Physics");
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			// Retrieve transform from Box2D
			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view) {
				Entity entity = { e, this };
				TransformComponent& transform = entity.getComponent<TransformComponent>();
				Rigidbody2DComponent& rb2d = entity.getComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rb2d.runtimeBody;
				const auto& position = body->GetPosition();
				transform.translation.x = position.x;
				transform.translation.y = position.y;
				transform.rotation.z = body->GetAngle();
			}
		}

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();

			for (auto entity : view) {
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.primary) {
					mainCamera = &camera.camera;
					cameraTransform = transform.getTransform();
					break;
				}
			}
		}

		if (mainCamera) {
			Renderer2D::beginScene(*mainCamera, cameraTransform);

			// draw sprites
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (entt::entity entity : group) {
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
				}
			}

			// draw circles
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (entt::entity entity : view) {
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
					Renderer2D::drawCircle(transform.getTransform(), circle.color, circle.thickness, circle.fade, (int)entity);
				}
			}

			Renderer2D::endScene();
		}
	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera) {
		Renderer2D::beginScene(camera);
		
		// draw sprites
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (entt::entity entity : group) {
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
			}
		}

		// draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (entt::entity entity : view) {
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::drawCircle(transform.getTransform(), circle.color, circle.thickness, circle.fade, (int)entity);
			}
		}
		
		Renderer2D::endScene();
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height) {
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view) {
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.setViewportSize(width, height);
		}
	}

	void Scene::duplicateEntity(Entity entity) {
		std::string name = entity.getName();
		Entity newEntity = createEntity(name);

		copyComponentIfExists<TransformComponent>(newEntity, entity);
		copyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		copyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		copyComponentIfExists<CameraComponent>(newEntity, entity);
		copyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		copyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		copyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		copyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
	}

	Entity Scene::getPrimaryCameraEntity() {
		auto view = m_Registry.view<CameraComponent>();
		for (entt::entity entity : view) {
			const CameraComponent& camera = view.get<CameraComponent>(entity);
			if (camera.primary)
				return Entity{ entity, this };
		}
		return {};
	}

	template<typename T>
	void Scene::onComponentAdded(Entity entity, T& component) {
		//static_assert(false);
	}

	template<>
	void Scene::onComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component) {

	}

	template<>
	void Scene::onComponentAdded<IDComponent>(Entity entity, IDComponent& component) {

	}

	template<>
	void Scene::onComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component) {

	}

	template<>
	void Scene::onComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component) {

	}

	template<>
	void Scene::onComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component) {

	}

	template<>
	void Scene::onComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {
		
	}

	template<>
	void Scene::onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.camera.setViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::onComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) {

	}

	template<>
	void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component) {

	}

	template<>
	void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {

	}
}
