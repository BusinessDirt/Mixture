#pragma once

/**
 * @file Entity.hpp
 * @brief High-level Entity wrapper around Flecs entity handles.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Util/UUID.hpp"
#include "Mixture/Scene/Components.hpp"

#include <flecs.h>
#include <string>
#include <vector>

namespace Mixture
{
    class Scene;

    /**
     * @brief Lightweight handle representing an entity in a Scene.
     */
    class Entity
    {
    public:
        Entity() = default;
        Entity(flecs::entity handle, Scene* scene);
        Entity(const Entity& other) = default;

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            OPAL_ASSERT("Scene", !HasComponent<T>(), "Entity already has component!");
            m_Handle.set<T>(T{ std::forward<Args>(args)... });
            return m_Handle.get_mut<T>();
        }

        template<typename T, typename... Args>
        T& AddOrReplaceComponent(Args&&... args)
        {
            m_Handle.set<T>(T{ std::forward<Args>(args)... });
            return m_Handle.get_mut<T>();
        }

        template<typename T>
        T& GetComponent()
        {
            OPAL_ASSERT("Scene", HasComponent<T>(), "Entity does not have component!");
            return m_Handle.get_mut<T>();
        }

        template<typename T>
        const T& GetComponent() const
        {
            OPAL_ASSERT("Scene", HasComponent<T>(), "Entity does not have component!");
            return m_Handle.get<T>();
        }

        template<typename T>
        T* TryGetComponent()
        {
            return m_Handle.try_get_mut<T>();
        }

        template<typename T>
        const T* TryGetComponent() const
        {
            return m_Handle.try_get<T>();
        }

        template<typename T>
        bool HasComponent() const
        {
            if (!m_Handle.is_valid()) return false;
            return m_Handle.has<T>();
        }

        template<typename T>
        void RemoveComponent()
        {
            OPAL_ASSERT("Scene", HasComponent<T>(), "Entity does not have component!");
            m_Handle.remove<T>();
        }

        flecs::entity GetFlecsEntity() const { return m_Handle; }
        uint64_t GetID() const { return static_cast<uint64_t>(m_Handle.id()); }
        
        UUID GetUUID() const;
        const std::string& GetName() const;
        void SetName(const std::string& name);

        bool IsValid() const { return m_Handle.is_valid() && m_Handle.is_alive(); }
        explicit operator bool() const { return IsValid(); }
        operator uint64_t() const { return GetID(); }
        operator flecs::entity() const { return m_Handle; }

        bool operator==(const Entity& other) const { return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }
        bool operator!=(const Entity& other) const { return !(*this == other); }

        // Hierarchy support using Flecs parent-child relations
        void SetParent(Entity parent);
        Entity GetParent() const;
        std::vector<Entity> GetChildren() const;
        void RemoveParent();
        bool HasParent() const;

        Scene* GetScene() const { return m_Scene; }

    private:
        flecs::entity m_Handle;
        Scene* m_Scene = nullptr;
    };
}
