#include <gtest/gtest.h>
#include "Mixture/Util/Util.hpp"

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
}
