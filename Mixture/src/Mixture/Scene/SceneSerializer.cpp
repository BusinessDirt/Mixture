#include "mxpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace YAML {
	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Mixture {
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static std::string rigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType) {
		switch (bodyType) {
			case Rigidbody2DComponent::BodyType::Static:    return "Static";
			case Rigidbody2DComponent::BodyType::Dynamic:   return "Dynamic";
			case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		MX_CORE_ASSERT(false, "Unknown body type");
		return {};
	}

	static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString) {
		if (bodyTypeString == "Static")    return Rigidbody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic")   return Rigidbody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

		MX_CORE_ASSERT(false, "Unknown body type");
		return Rigidbody2DComponent::BodyType::Static;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene) {}

	static void serializeEntity(YAML::Emitter& out, Entity entity) {
		MX_CORE_ASSERT(entity.hasComponent<IDComponent>());

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.getUUID(); 

		if (entity.hasComponent<TagComponent>()) {
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			std::string& tag = entity.getComponent<TagComponent>().tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<TransformComponent>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			TransformComponent& transformComponent = entity.getComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << transformComponent.translation;
			out << YAML::Key << "Rotation" << YAML::Value << transformComponent.rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComponent.scale;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<CameraComponent>()) {
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent
			
			CameraComponent& cameraComponent = entity.getComponent<CameraComponent>();
			SceneCamera& camera = cameraComponent.camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.getProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.getPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.getPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.getPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.getOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.getOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.getOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.fixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.hasComponent<SpriteRendererComponent>()) {
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			SpriteRendererComponent& spriteRendererComponent = entity.getComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.color;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.hasComponent<CircleRendererComponent>()) {
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // CircleRendererComponent

			CircleRendererComponent& circleRendererComponent = entity.getComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.color;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.fade;

			out << YAML::EndMap; // CircleRendererComponent
		}

		if (entity.hasComponent<Rigidbody2DComponent>()) {
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2DComponent

			Rigidbody2DComponent& rb2dComponent = entity.getComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << rigidBody2DBodyTypeToString(rb2dComponent.type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.fixedRotation;

			out << YAML::EndMap; // Rigidbody2DComponent
		}

		if (entity.hasComponent<BoxCollider2DComponent>()) {
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			BoxCollider2DComponent& bc2dComponent = entity.getComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.offset;
			out << YAML::Key << "Size" << YAML::Value << bc2dComponent.size;
			out << YAML::Key << "Density" << YAML::Value << bc2dComponent.density;
			out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.restitutionThreshold;

			out << YAML::EndMap; // BoxCollider2DComponent
		}

		if (entity.hasComponent<CircleCollider2DComponent>()) {
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent

			auto& cc2dComponent = entity.getComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << cc2dComponent.offset;
			out << YAML::Key << "Radius" << YAML::Value << cc2dComponent.radius;
			out << YAML::Key << "Density" << YAML::Value << cc2dComponent.density;
			out << YAML::Key << "Friction" << YAML::Value << cc2dComponent.friction;
			out << YAML::Key << "Restitution" << YAML::Value << cc2dComponent.restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2dComponent.restitutionThreshold;

			out << YAML::EndMap; // CircleCollider2DComponent
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::serialize(const std::string& filepath) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID) {
			Entity entity = { entityID, m_Scene.get() };
			if (!entity) return;

			serializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}
	
	void SceneSerializer::serializeRuntime(const std::string& filepath) {
		MX_CORE_ASSERT(false);
	}
	
	bool SceneSerializer::deserialize(const std::string& filepath) {
		YAML::Node data;
		try {
			data = YAML::LoadFile(filepath);
		} catch(YAML::ParserException e) {
			MX_CORE_ERROR("Failed to load .mxscene file '{0}'\n    {1}", filepath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		MX_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		YAML::Node entities = data["Entities"];
		if (entities) {
			for (YAML::Node entity : entities) {
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				YAML::Node tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				MX_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->createEntityWithUUID(uuid, name);

				YAML::Node transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					TransformComponent& tc = deserializedEntity.getComponent<TransformComponent>();
					tc.translation = transformComponent["Translation"].as<glm::vec3>();
					tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.scale = transformComponent["Scale"].as<glm::vec3>();
				}

				YAML::Node cameraComponent = entity["CameraComponent"];
				if (cameraComponent) {
					CameraComponent& cc = deserializedEntity.addComponent<CameraComponent>();

					YAML::Node cameraProps = cameraComponent["Camera"];
					cc.camera.setProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.camera.setPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.camera.setPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.camera.setPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.camera.setOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.camera.setOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.camera.setOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.primary = cameraComponent["Primary"].as<bool>();
					cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				YAML::Node spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent) {
					SpriteRendererComponent& src = deserializedEntity.addComponent<SpriteRendererComponent>();
					src.color = spriteRendererComponent["Color"].as<glm::vec4>();
				}

				YAML::Node circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent) {
					CircleRendererComponent& crc = deserializedEntity.addComponent<CircleRendererComponent>();
					crc.color = circleRendererComponent["Color"].as<glm::vec4>();
					crc.thickness = circleRendererComponent["Thickness"].as<float>();
					crc.fade = circleRendererComponent["Fade"].as<float>();
				}

				YAML::Node rigidbody2DComponent = entity["Rigidbody2DComponent"];
				if (rigidbody2DComponent) {
					Rigidbody2DComponent& rb2d = deserializedEntity.addComponent<Rigidbody2DComponent>();
					rb2d.type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
					rb2d.fixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
				}

				YAML::Node boxCollider2DComponent = entity["BoxCollider2DComponent"];
				if (boxCollider2DComponent) {
					BoxCollider2DComponent& bc2d = deserializedEntity.addComponent<BoxCollider2DComponent>();
					bc2d.offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2d.size = boxCollider2DComponent["Size"].as<glm::vec2>();
					bc2d.density = boxCollider2DComponent["Density"].as<float>();
					bc2d.friction = boxCollider2DComponent["Friction"].as<float>();
					bc2d.restitution = boxCollider2DComponent["Restitution"].as<float>();
					bc2d.restitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
				}


				YAML::Node circleCollider2DComponent = entity["CircleCollider2DComponent"];
				if (circleCollider2DComponent) {
					CircleCollider2DComponent& cc2d = deserializedEntity.addComponent<CircleCollider2DComponent>();
					cc2d.offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					cc2d.radius = circleCollider2DComponent["Radius"].as<float>();
					cc2d.density = circleCollider2DComponent["Density"].as<float>();
					cc2d.friction = circleCollider2DComponent["Friction"].as<float>();
					cc2d.restitution = circleCollider2DComponent["Restitution"].as<float>();
					cc2d.restitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
				}
			}
		}

		return true;
	}
	
	bool SceneSerializer::deserializeRuntime(const std::string& filepath) {
		MX_CORE_ASSERT(false);
		return false;
	}
}


