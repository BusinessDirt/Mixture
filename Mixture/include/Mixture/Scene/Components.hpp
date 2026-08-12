#pragma once

/**
 * @file Components.hpp
 * @brief Core component definitions for the Mixture ECS architecture.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Util/UUID.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <string>

namespace Mixture
{
    /**
     * @brief Component storing a Universally Unique Identifier for entity persistence and serialization.
     */
    struct IDComponent
    {
        UUID ID;

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
        IDComponent(const UUID& id) : ID(id) {}
    };

    /**
     * @brief Component storing tag/name and active state of an entity.
     */
    struct TagComponent
    {
        std::string Name = "Entity";
        bool Active = true;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& name, bool active = true)
            : Name(name), Active(active) {}
    };

    /**
     * @brief Component representing spatial transformation (Position, Rotation, Scale).
     */
    struct TransformComponent
    {
        glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
        glm::quat Rotation{ 1.0f, 0.0f, 0.0f, 0.0f }; // Identity quaternion
        glm::vec3 Scale{ 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& position)
            : Position(position) {}
        TransformComponent(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale = glm::vec3(1.0f))
            : Position(position), Rotation(rotation), Scale(scale) {}

        /**
         * @brief Sets rotation from Euler angles (in radians).
         */
        void SetRotationEuler(const glm::vec3& eulerRadians)
        {
            Rotation = glm::quat(eulerRadians);
        }

        /**
         * @brief Gets rotation as Euler angles (in radians).
         */
        glm::vec3 GetRotationEuler() const
        {
            return glm::eulerAngles(Rotation);
        }

        /**
         * @brief Calculates local TRS (Translation * Rotation * Scale) matrix.
         */
        glm::mat4 GetTransform() const
        {
            return glm::translate(glm::mat4(1.0f), Position)
                * glm::toMat4(Rotation)
                * glm::scale(glm::mat4(1.0f), Scale);
        }
    };

    /**
     * @brief Component for mesh rendering and surface material properties.
     */
    struct MeshRendererComponent
    {
        glm::vec4 MaterialColor{ 1.0f, 1.0f, 1.0f, 1.0f };
        float Roughness = 0.5f;
        float Metallic = 0.0f;
        std::string MeshPath = "";
        bool Enabled = true;

        MeshRendererComponent() = default;
        MeshRendererComponent(const MeshRendererComponent&) = default;
    };

    /**
     * @brief Types of light sources.
     */
    enum class LightType
    {
        Directional = 0,
        Point = 1,
        Spot = 2
    };

    /**
     * @brief Component representing a light source in the scene.
     */
    struct LightComponent
    {
        LightType Type = LightType::Point;
        glm::vec3 Color{ 1.0f, 1.0f, 1.0f };
        float Intensity = 1.0f;
        float Range = 10.0f;
        float SpotAngle = 45.0f;
        bool Enabled = true;

        LightComponent() = default;
        LightComponent(const LightComponent&) = default;
    };

    /**
     * @brief Component defining camera projection and parameters.
     */
    struct CameraComponent
    {
        float Fov = 45.0f;
        float NearClip = 0.1f;
        float FarClip = 1000.0f;
        bool Primary = true;
        bool FixedAspectRatio = false;
        float AspectRatio = 1.778f;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };
}
