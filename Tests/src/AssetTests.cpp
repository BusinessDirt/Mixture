#include <gtest/gtest.h>
#include "Mixture/Assets/AssetRegistry.hpp"

using namespace Mixture;

class AssetRegistryTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        AssetRegistry::Get().Clear();
    }

    void TearDown() override
    {
        AssetRegistry::Get().Clear();
    }
};

TEST_F(AssetRegistryTests, NoRedirect)
{
    std::filesystem::path p("Texture.png");
    EXPECT_EQ(AssetRegistry::Get().ResolvePath(AssetType::Texture, p), p);
}

TEST_F(AssetRegistryTests, SimpleRedirect)
{
    AssetRegistry::Get().AddRedirector(AssetType::Texture, "Old.png", "New.png");
    
    std::filesystem::path resolved = AssetRegistry::Get().ResolvePath(AssetType::Texture, "Old.png");
    EXPECT_EQ(resolved, std::filesystem::path("New.png"));
}

TEST_F(AssetRegistryTests, ChainedRedirect)
{
    AssetRegistry::Get().AddRedirector(AssetType::Texture, "A.png", "B.png");
    AssetRegistry::Get().AddRedirector(AssetType::Texture, "B.png", "C.png");

    std::filesystem::path resolved = AssetRegistry::Get().ResolvePath(AssetType::Texture, "A.png");
    EXPECT_EQ(resolved, std::filesystem::path("C.png"));
}

TEST_F(AssetRegistryTests, LoopDetection)
{
    // A -> B -> A
    AssetRegistry::Get().AddRedirector(AssetType::Texture, "A.png", "B.png");
    AssetRegistry::Get().AddRedirector(AssetType::Texture, "B.png", "A.png");

    // Should return one of them and not crash, and log an error (which we can't easily verify here without a mock logger)
    // Based on implementation: 
    // i=0: A->B
    // i=1: B->A
    // ...
    // i=9: B->A
    // Returns A
    
    std::filesystem::path resolved = AssetRegistry::Get().ResolvePath(AssetType::Texture, "A.png");
    EXPECT_FALSE(resolved.empty());
}

TEST_F(AssetRegistryTests, TypeSeparation)
{
    AssetRegistry::Get().AddRedirector(AssetType::Texture, "X", "Y");
    
    // Should not affect Mesh type
    EXPECT_EQ(AssetRegistry::Get().ResolvePath(AssetType::Mesh, "X"), std::filesystem::path("X"));
    
    // Should affect Texture type
    EXPECT_EQ(AssetRegistry::Get().ResolvePath(AssetType::Texture, "X"), std::filesystem::path("Y"));
}
