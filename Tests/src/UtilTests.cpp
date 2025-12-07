#include <gtest/gtest.h>
#include "Mixture/Util/Util.hpp"
#include "Mixture/Util/ToString.hpp"

namespace Mixture::Tests {

    // --- Util.hpp Tests ---

    TEST(UtilTests, HashCombine) {
        std::size_t seed = 0;
        Util::HashCombine(seed, 42);
        std::size_t seed1 = seed;
        
        EXPECT_NE(seed, 0);

        seed = 0;
        Util::HashCombine(seed, 42);
        EXPECT_EQ(seed, seed1); // Deterministic

        Util::HashCombine(seed, 100);
        EXPECT_NE(seed, seed1); // Changes with new value
    }

    TEST(UtilTests, Contains) {
        std::string str = "Hello World";
        EXPECT_TRUE(Util::Contains(str, "World"));
        EXPECT_TRUE(Util::Contains(str, "Hello"));
        EXPECT_FALSE(Util::Contains(str, "Universe"));
        EXPECT_FALSE(Util::Contains(str, "world")); // Case sensitive usually
    }

    // --- ToString.hpp Tests ---
    // Note: These test the mapping. If mappings change, tests need update.
    
    TEST(UtilTests, VulkanToString) {
        // Just spot check a few to ensure linkage and basic logic
        EXPECT_EQ(Vulkan::ToString::DescriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER), "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER");
        EXPECT_EQ(Vulkan::ToString::Format(VK_FORMAT_R8G8B8A8_UNORM), "VK_FORMAT_R8G8B8A8_UNORM");
        
        // Test unknown/default
        // The implementation usually handles this, we assume it doesn't crash.
    }
}
