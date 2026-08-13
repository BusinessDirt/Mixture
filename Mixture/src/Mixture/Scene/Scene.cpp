#include "Mixture/Scene/Scene.hpp"
#include "Mixture/Scene/Entity.hpp"

namespace Mixture
{
    Scene::Scene(const std::string& name)
        : m_Name(name)
    {
    }

    Ref<Scene> Scene::Create(const std::string& name)
    {
        return CreateRef<Scene>(name);
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        return CreateEntityWithUUID(UUID(), name);
    }

    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
    {
        flecs::entity handle = m_World.entity();
        Entity entity(handle, this);

        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
        entity.AddComponent<TransformComponent>();

        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        if (entity.IsValid())
        {
            entity.GetFlecsEntity().destruct();
        }
    }

    Entity Scene::GetEntityByUUID(UUID uuid)
    {
        Entity result{};
        m_World.each([&](flecs::entity e, const IDComponent& idComp) {
            if (idComp.ID == uuid)
            {
                result = Entity(e, this);
            }
        });
        return result;
    }

    Entity Scene::GetEntityByName(std::string_view name)
    {
        Entity result{};
        m_World.each([&](flecs::entity e, const TagComponent& tagComp) {
            if (tagComp.Name == name)
            {
                result = Entity(e, this);
            }
        });
        return result;
    }

    Entity Scene::GetEntityByFlecsID(uint64_t id)
    {
        flecs::entity handle = m_World.entity(static_cast<flecs::entity_t>(id));
        if (handle.is_valid() && handle.is_alive())
        {
            return Entity(handle, this);
        }
        return Entity{};
    }

    std::vector<Entity> Scene::GetAllEntities()
    {
        std::vector<Entity> entities;
        m_World.each([&](flecs::entity e, const IDComponent&) {
            if (e.is_valid() && e.is_alive())
            {
                entities.push_back(Entity(e, this));
            }
        });
        return entities;
    }

    std::vector<Entity> Scene::GetRootEntities()
    {
        std::vector<Entity> rootEntities;
        m_World.each([&](flecs::entity e, const IDComponent&) {
            if (e.is_valid() && e.is_alive())
            {
                flecs::entity parent = e.parent();
                if (!parent.is_valid() || !parent.is_alive())
                {
                    rootEntities.push_back(Entity(e, this));
                }
            }
        });
        return rootEntities;
    }

    void Scene::OnUpdate(float dt)
    {
        m_World.progress(dt);
    }

    void Scene::OnRender()
    {
        // Render processing pass
    }
}
