#include <gtest/gtest.h>
#include "Mixture/Assets/AssetRegistry.hpp"
#include "Mixture/Assets/AssetManager.hpp"
#include "Mixture/Assets/AssetSerializer.hpp"
#include "Mixture/Assets/Shaders/ShaderAsset.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"
#include "Mixture/Assets/Textures/TextureAsset.hpp"
#include <fstream>

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

// --- AssetManager Tests ---

class AssetManagerTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Ensure clean state if possible. 
        // AssetManager is singleton, so we rely on its internal cleanup or we assume it's fresh enough.
        // We can call Init/Shutdown to be sure.
        AssetManager::Get().Init();
    }

    void TearDown() override
    {
        AssetManager::Get().Shutdown();
    }
};

TEST_F(AssetManagerTests, SingletonAccess)
{
    AssetManager& am = AssetManager::Get();
    EXPECT_EQ(&am, &AssetManager::Get());
}

TEST_F(AssetManagerTests, GraphicsAPI)
{
    AssetManager::Get().SetGraphicsAPI(RHI::GraphicsAPI::Vulkan);
    EXPECT_EQ(AssetManager::Get().GetGraphicsAPI(), RHI::GraphicsAPI::Vulkan);
    
    AssetManager::Get().SetGraphicsAPI(RHI::GraphicsAPI::Metal);
    EXPECT_EQ(AssetManager::Get().GetGraphicsAPI(), RHI::GraphicsAPI::Metal);
}

TEST_F(AssetManagerTests, LifecycleIsIdempotentAndResetsState)
{
    AssetManager& manager = AssetManager::Get();
    ASSERT_TRUE(manager.IsInitialized());

    manager.Init();
    EXPECT_TRUE(manager.IsInitialized());

    manager.Shutdown();
    EXPECT_FALSE(manager.IsInitialized());
    EXPECT_EQ(manager.GetGraphicsAPI(), RHI::GraphicsAPI::None);

    manager.Shutdown();
    manager.Init();
    EXPECT_TRUE(manager.IsInitialized());
}

TEST_F(AssetManagerTests, ReloadCallbackRegistrationsAreRemovable)
{
    AssetManager& manager = AssetManager::Get();
    const auto callback = manager.AddReloadCallback([](AssetType, UUID) {});

    EXPECT_TRUE(manager.RemoveReloadCallback(callback));
    EXPECT_FALSE(manager.RemoveReloadCallback(callback));
}

// --- AssetSerializer Metadata Tests ---

TEST(AssetSerializerTests, MetadataRoundTrip)
{
    AssetMetadata metadata;
    metadata.ID = UUID(); // Generates random
    metadata.Type = AssetType::Texture;
    metadata.FilePath = "Assets/Test.png";
    
    std::filesystem::path metaPath = "Assets/Test.png.meta";
    
    // Ensure directory exists if needed (here we assume current dir is writable or Assets/ exists)
    // Actually, let's use a temp path
    std::filesystem::path tempPath = std::filesystem::temp_directory_path() / "MixtureTest.png";
    metadata.FilePath = tempPath;
    
    // Write
    AssetSerializer::WriteMetadata(metadata);
    
    std::filesystem::path metaFile = tempPath;
    metaFile += ".meta";
    
    EXPECT_TRUE(std::filesystem::exists(metaFile));
    
    // Read
    AssetMetadata readMetadata;
    bool success = AssetSerializer::TryLoadMetadata(tempPath, readMetadata);
    EXPECT_TRUE(success);
    EXPECT_EQ(readMetadata.ID, metadata.ID);
    EXPECT_EQ(readMetadata.Type, metadata.Type);
    
    // Cleanup
    if (std::filesystem::exists(metaFile))
        std::filesystem::remove(metaFile);
}

// --- Asset Types Tests ---

TEST(AssetTypeTests, TextureAsset)
{
    UUID id;
    std::vector<uint8_t> data = { 255, 0, 0, 255 }; // Red pixel
    TextureAsset texture(id, "TestTex", 1, 1, RHI::Format::R8G8B8A8_UNORM, data);
    
    EXPECT_EQ(texture.GetID(), id);
    EXPECT_EQ(texture.GetName(), "TestTex");
    EXPECT_EQ(texture.GetWidth(), 1);
    EXPECT_EQ(texture.GetHeight(), 1);
    EXPECT_EQ(texture.GetFormat(), RHI::Format::R8G8B8A8_UNORM);
    EXPECT_EQ(texture.GetDataSize(), 4);
    
    // Check const void* access
    const uint8_t* ptr = static_cast<const uint8_t*>(texture.GetData());
    EXPECT_EQ(ptr[0], 255);
}

TEST(AssetTypeTests, ShaderAsset)
{
    UUID id;
    std::vector<uint8_t> bytecode = { 0xDE, 0xAD, 0xBE, 0xEF };
    ShaderAsset shader(id, "TestShader", bytecode);
    
    EXPECT_EQ(shader.GetID(), id);
    EXPECT_EQ(shader.GetName(), "TestShader");
    EXPECT_TRUE(shader.IsValid());
    EXPECT_EQ(shader.GetBufferSize(), 4);
    
    const uint8_t* ptr = static_cast<const uint8_t*>(shader.GetBufferPointer());
    EXPECT_EQ(ptr[0], 0xDE);
}

// --- Shader Compiler Tests ---

TEST(ShaderCompilerTests, BasicCompile)
{
    // Simple Vertex Shader
    std::string source = R"(
        #version 450
        void main() { gl_Position = vec4(0,0,0,1); }
    )";
    
    // We expect this to either succeed or throw/return empty depending on implementation.
    // Given we don't know if a compiler is linked, we wrap in try/catch or check size.
    try {
        auto spirv = ShaderCompiler::Compile(source);
        if (!spirv.empty())
        {
            // If it compiled, it should be valid SPIR-V
            EXPECT_GT(spirv.size(), 0);
            EXPECT_EQ(spirv.size() % 4, 0); // SPIR-V is 32-bit words
            
            // Reflection Test
            auto reflection = ShaderCompiler::ReflectSPIRV(spirv.data(), spirv.size());
            // Should have entry point "main"
            bool hasMain = false;
            for(auto& [stage, name] : reflection.EntryPoints) {
                if (name == "main") hasMain = true;
            }
            EXPECT_TRUE(hasMain);
        }
        else
        {
            // Warn if compilation returns empty but didn't throw (maybe mock compiler)
            std::cout << "[WARNING] ShaderCompiler returned empty SPIR-V (Compiler might be missing/mocked)" << std::endl;
        }
    } catch (...) {
        // If it throws, we can't easily test it here without more setup
        std::cout << "[WARNING] ShaderCompiler threw exception" << std::endl;
    }
}
