#include "Mixture/Scene/Entity.hpp"
#include "Mixture/Scene/Scene.hpp"

namespace Mixture
{
    Entity::Entity(flecs::entity handle, Scene* scene)
        : m_Handle(handle), m_Scene(scene)
    {
    }

    UUID Entity::GetUUID() const
    {
        if (HasComponent<IDComponent>())
        {
            return GetComponent<IDComponent>().ID;
        }
        return UUID::Invalid();
    }

    const std::string& Entity::GetName() const
    {
        if (HasComponent<TagComponent>())
        {
            return GetComponent<TagComponent>().Name;
        }
        static const std::string s_EmptyStr = "Entity";
        return s_EmptyStr;
    }

    void Entity::SetName(const std::string& name)
    {
        if (HasComponent<TagComponent>())
        {
            GetComponent<TagComponent>().Name = name;
        }
        else
        {
            m_Handle.set<TagComponent>(TagComponent{ name });
        }
    }

    void Entity::SetParent(Entity parent)
    {
        if (parent.IsValid())
        {
            m_Handle.child_of(parent.GetFlecsEntity());
        }
    }

    Entity Entity::GetParent() const
    {
        if (!IsValid()) return Entity{};

        flecs::entity parentHandle = m_Handle.parent();
        if (parentHandle.is_valid() && parentHandle.is_alive())
        {
            return Entity(parentHandle, m_Scene);
        }
        return Entity{};
    }

    bool Entity::HasParent() const
    {
        if (!IsValid()) return false;

        flecs::entity parentHandle = m_Handle.parent();
        return parentHandle.is_valid() && parentHandle.is_alive();
    }

    void Entity::RemoveParent()
    {
        if (!IsValid()) return;

        flecs::entity parentHandle = m_Handle.parent();
        if (parentHandle.is_valid())
        {
            m_Handle.remove(flecs::ChildOf, parentHandle);
        }
    }

    std::vector<Entity> Entity::GetChildren() const
    {
        std::vector<Entity> children;
        if (IsValid())
        {
            m_Handle.children([&](flecs::entity childHandle) {
                if (childHandle.is_valid() && childHandle.is_alive())
                {
                    children.push_back(Entity(childHandle, m_Scene));
                }
            });
        }
        return children;
    }
}
