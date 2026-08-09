#include <gtest/gtest.h>
#include "Mixture/Core/Memory/LRUCache.hpp"
#include "Mixture/Core/Memory/ArenaAllocator.hpp"
#include "Mixture/Core/Memory/PoolAllocator.hpp"

namespace Mixture::Tests {

    // --- LRUCache Tests ---

    TEST(MemoryTests, LRUCacheBasic) {
        LRUCache<int, std::string> cache(100); // 100 bytes limit

        cache.Put(1, "One", 10);
        EXPECT_TRUE(cache.Contains(1));
        EXPECT_EQ(cache.Get(1), "One");
        EXPECT_EQ(cache.GetUsage(), 10);
        EXPECT_EQ(cache.GetCount(), 1);

        cache.Put(2, "Two", 20);
        EXPECT_TRUE(cache.Contains(2));
        EXPECT_EQ(cache.GetUsage(), 30); // 10 + 20
        EXPECT_EQ(cache.GetCount(), 2);
    }

    TEST(MemoryTests, LRUCacheEviction) {
        LRUCache<int, int> cache(30); // 30 bytes capacity

        // Fill cache
        cache.Put(1, 100, 10); // Usage 10
        cache.Put(2, 200, 10); // Usage 20
        cache.Put(3, 300, 10); // Usage 30

        EXPECT_EQ(cache.GetCount(), 3);
        EXPECT_EQ(cache.GetUsage(), 30);

        // Access 1 to make it MRU (Most Recently Used)
        // Order: 2, 3, 1
        EXPECT_EQ(cache.Get(1), 100);

        // Insert new item causing eviction
        // Should evict LRU item, which is 2 (since 1 was just accessed, and 3 was inserted after 2)
        // Wait:
        // Put 1 -> List: 1
        // Put 2 -> List: 2, 1
        // Put 3 -> List: 3, 2, 1 (Front is MRU)
        // Get 1 -> List: 1, 3, 2
        // Put 4 (size 10) -> Evict LRU (Back) -> 2
        cache.Put(4, 400, 10); 
        
        EXPECT_FALSE(cache.Contains(2));
        EXPECT_TRUE(cache.Contains(1));
        EXPECT_TRUE(cache.Contains(3));
        EXPECT_TRUE(cache.Contains(4));
        EXPECT_EQ(cache.GetUsage(), 30);
    }

    TEST(MemoryTests, LRUCacheUpdate) {
        LRUCache<int, int> cache(50);
        
        cache.Put(1, 10, 10);
        EXPECT_EQ(cache.Get(1), 10);
        EXPECT_EQ(cache.GetUsage(), 10);

        // Update value and size
        cache.Put(1, 20, 20);
        EXPECT_EQ(cache.Get(1), 20);
        EXPECT_EQ(cache.GetUsage(), 20); // Should update size
        EXPECT_EQ(cache.GetCount(), 1);
    }

    TEST(MemoryTests, LRUCacheEvictionCallback) {
        LRUCache<int, int> cache(20);
        
        int evictedKey = -1;
        int evictedValue = -1;
        
        cache.SetEvictionCallback([&](const int& k, const int& v) {
            evictedKey = k;
            evictedValue = v;
        });

        cache.Put(1, 10, 15);
        cache.Put(2, 20, 10); // 15+10 = 25 > 20. Evict 1.

        EXPECT_EQ(evictedKey, 1);
        EXPECT_EQ(evictedValue, 10);
        EXPECT_FALSE(cache.Contains(1));
        EXPECT_TRUE(cache.Contains(2));
    }

    TEST(MemoryTests, LRUCacheSetMaxMemory) {
        LRUCache<int, int> cache(100);
        cache.Put(1, 10, 40);
        cache.Put(2, 20, 40);
        EXPECT_EQ(cache.GetUsage(), 80);
        
        // Lower limit to trigger eviction
        cache.SetMaxMemory(50);
        
        // Should evict one (LRU is 1)
        EXPECT_LE(cache.GetUsage(), 50);
        EXPECT_EQ(cache.GetCount(), 1);
        EXPECT_TRUE(cache.Contains(2));
        EXPECT_FALSE(cache.Contains(1));
    }

    TEST(MemoryTests, LRUEvictionDoesNotCopyValue) {
        struct CopyCounted {
            int* Copies = nullptr;
            explicit CopyCounted(int* copies = nullptr) : Copies(copies) {}
            CopyCounted(const CopyCounted& other) : Copies(other.Copies) { if (Copies) ++*Copies; }
            CopyCounted(CopyCounted&&) noexcept = default;
            CopyCounted& operator=(const CopyCounted& other) {
                Copies = other.Copies;
                if (Copies) ++*Copies;
                return *this;
            }
            CopyCounted& operator=(CopyCounted&&) noexcept = default;
        };

        int copies = 0;
        LRUCache<int, CopyCounted> cache(1);
        cache.SetEvictionCallback([](const int&, const CopyCounted&) {});
        cache.Put(1, CopyCounted(&copies), 1);
        cache.Put(2, CopyCounted(&copies), 1);

        EXPECT_EQ(copies, 0);
        EXPECT_FALSE(cache.Contains(1));
        EXPECT_TRUE(cache.Contains(2));
    }
    
    // --- ArenaAllocator Tests ---

    TEST(MemoryTests, ArenaAllocatorBasic) {
        ArenaAllocator arena(1024);
        
        int* val1 = arena.Alloc<int>(10);
        EXPECT_NE(val1, nullptr);
        EXPECT_EQ(*val1, 10);
        
        float* val2 = arena.Alloc<float>(20.5f);
        EXPECT_NE(val2, nullptr);
        EXPECT_FLOAT_EQ(*val2, 20.5f);
        
        EXPECT_GT(arena.GetUsedMemory(), 0);
        
        arena.Reset();
        EXPECT_EQ(arena.GetUsedMemory(), 0);
        
        // Re-alloc after reset
        int* val3 = arena.Alloc<int>(99);
        EXPECT_NE(val3, nullptr);
        EXPECT_EQ(*val3, 99);
        // Should reuse start
        EXPECT_EQ((void*)val3, (void*)val1);
    }

    // --- PoolAllocator Tests ---

    struct TestObject {
        int x, y;
        TestObject(int a, int b) : x(a), y(b) {}
    };

    TEST(MemoryTests, PoolAllocatorBasic) {
        PoolAllocator pool(sizeof(TestObject), alignof(TestObject), 5);
        
        TestObject* obj1 = pool.Create<TestObject>(1, 2);
        EXPECT_NE(obj1, nullptr);
        EXPECT_EQ(obj1->x, 1);
        
        TestObject* obj2 = pool.Create<TestObject>(3, 4);
        EXPECT_NE(obj2, nullptr);
        
        EXPECT_EQ(pool.GetUsedCount(), 2);
        
        pool.Destroy(obj1);
        EXPECT_EQ(pool.GetUsedCount(), 1);
        
        // Next alloc should likely reuse obj1's slot (LIFO free list)
        TestObject* obj3 = pool.Create<TestObject>(5, 6);
        EXPECT_EQ(obj3, obj1); 
        EXPECT_EQ(pool.GetUsedCount(), 2);
        
        pool.Reset();
        EXPECT_EQ(pool.GetUsedCount(), 0);
    }

}
