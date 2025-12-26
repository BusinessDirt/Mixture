#pragma once

#include "Mixture/Core/Base.hpp"
#include <list>
#include <unordered_map>
#include <functional>

namespace Mixture
{
    /**
     * @brief A generic Least Recently Used (LRU) cache with memory limit.
     * 
     * Keeps track of memory usage and evicts the least recently used items
     * when the limit is exceeded.
     */
    template<typename Key, typename Value>
    class LRUCache
    {
    public:
        using EvictionCallback = std::function<void(const Key&, const Value&)>;

        /**
         * @brief Constructs an LRU Cache.
         * @param maxMemoryUsage The maximum allowed memory usage in bytes.
         */
        LRUCache(size_t maxMemoryUsage)
            : m_MaxMemoryUsage(maxMemoryUsage), m_CurrentMemoryUsage(0)
        {}

        /**
         * @brief Sets the maximum memory limit. Triggers eviction if needed.
         */
        void SetMaxMemory(size_t maxBytes) { m_MaxMemoryUsage = maxBytes; EvictIfNeeded(); }

        /**
         * @brief Gets the maximum memory limit.
         */
        size_t GetMaxMemory() const { return m_MaxMemoryUsage; }

        /**
         * @brief Retrieves an item from the cache and marks it as most recently used.
         * @return The value, or a default constructed value (nullptr for pointers) if not found.
         */
        Value Get(const Key& key)
        {
            auto it = m_Map.find(key);
            if (it == m_Map.end()) return Value{};

            // Move to front (Most Recently Used)
            m_List.splice(m_List.begin(), m_List, it->second);
            return it->second->Val;
        }

        /**
         * @brief Inserts or updates an item in the cache.
         * @param key The key.
         * @param value The value.
         * @param size The size of the item in bytes.
         */
        void Put(const Key& key, const Value& value, size_t size)
        {
            auto it = m_Map.find(key);
            if (it != m_Map.end())
            {
                // Update existing
                m_CurrentMemoryUsage -= it->second->Size;
                m_CurrentMemoryUsage += size;
                it->second->Val = value;
                it->second->Size = size;
                m_List.splice(m_List.begin(), m_List, it->second);
            }
            else
            {
                // Insert new
                m_List.push_front({ key, value, size });
                m_Map[key] = m_List.begin();
                m_CurrentMemoryUsage += size;
            }

            EvictIfNeeded();
        }

        /**
         * @brief Checks if the key exists in the cache without updating LRU status.
         */
        bool Contains(const Key& key) const
        {
            return m_Map.find(key) != m_Map.end();
        }

        /**
         * @brief Sets a callback to be called when an item is evicted.
         */
        void SetEvictionCallback(EvictionCallback callback)
        {
            m_EvictionCallback = callback;
        }

        /**
         * @brief Gets the current memory usage.
         */
        size_t GetUsage() const { return m_CurrentMemoryUsage; }

        /**
         * @brief Gets the number of items in the cache.
         */
        size_t GetCount() const { return m_Map.size(); }

        /**
         * @brief Clears the cache.
         */
        void Clear()
        {
            m_List.clear();
            m_Map.clear();
            m_CurrentMemoryUsage = 0;
        }

    private:
        void EvictIfNeeded()
        {
            while (m_CurrentMemoryUsage > m_MaxMemoryUsage && !m_List.empty())
            {
                auto last = m_List.back();
                
                if (m_EvictionCallback)
                    m_EvictionCallback(last.K, last.Val);

                m_CurrentMemoryUsage -= last.Size;
                m_Map.erase(last.K);
                m_List.pop_back();
            }
        }

        struct CacheEntry
        {
            Key K;
            Value Val;
            size_t Size;
        };

        size_t m_MaxMemoryUsage;
        size_t m_CurrentMemoryUsage;
        
        std::list<CacheEntry> m_List;
        std::unordered_map<Key, typename std::list<CacheEntry>::iterator> m_Map;
        EvictionCallback m_EvictionCallback;
    };
}
