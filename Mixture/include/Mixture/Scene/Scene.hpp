#pragma once

/**
 * @file Scene.hpp
 * @brief Represents a 3D scene holding entities, components, and systems managed by Flecs.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Util/UUID.hpp"
#include "Mixture/Scene/Components.hpp"
#include "Mixture/Scene/Entity.hpp"

#include <flecs.h>
#include <string>
#include <vector>
#include <string_view>

namespace Mixture
{
    /**
     * @brief Container class representing an active scene containing entities and ECS systems.
     */
    class Scene
    {
    public:
        Scene(const std::string& name = "Untitled Scene");
        ~Scene() = default;

        /** Creates a new shared reference to a Scene instance. */
        static Ref<Scene> Create(const std::string& name = "Untitled Scene");

        /** Creates a new entity with auto-generated UUID, TagComponent, and TransformComponent. */
        Entity CreateEntity(const std::string& name = "Entity");

        /** Creates a new entity with a specified UUID, TagComponent, and TransformComponent. */
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = "Entity");

        /** Destroys an entity and all its child entities from the scene. */
        void DestroyEntity(Entity entity);

        /** Finds an entity by its UUID. Returns invalid Entity if not found. */
        Entity GetEntityByUUID(UUID uuid);

        /** Finds an entity by its name tag. Returns invalid Entity if not found. */
        Entity GetEntityByName(std::string_view name);

        /** Finds an entity by its Flecs entity ID. */
        Entity GetEntityByFlecsID(uint64_t id);

        /** Retrieves all entities currently registered in the scene. */
        std::vector<Entity> GetAllEntities();

        /** Retrieves top-level root entities (entities without a parent). */
        std::vector<Entity> GetRootEntities();

        /** Advance scene logic and run Flecs systems. */
        void OnUpdate(float dt);

        /** Process scene rendering passes and draw mesh entities. */
        void OnRender();

        /** Gets reference to underlying Flecs world instance. */
        flecs::world& GetFlecsWorld() { return m_World; }
        const flecs::world& GetFlecsWorld() const { return m_World; }

        /** Scene Name getter and setter. */
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        /**
         * @brief Iterates over all entities matching specified component types.
         */
        template<typename... Components, typename Func>
        void Each(Func&& func)
        {
            m_World.each<Components...>(std::forward<Func>(func));
        }

    private:
        flecs::world m_World;
        std::string m_Name;

        friend class Entity;
    };
}
