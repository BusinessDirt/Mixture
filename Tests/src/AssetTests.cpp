#include <gtest/gtest.h>
#include "Mixture/Assets/AssetRegistry.hpp"
#include "Mixture/Assets/AssetManager.hpp"
#include "Mixture/Assets/AssetSerializer.hpp"
#include "Mixture/Assets/Shaders/ShaderAsset.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"
#include "Mixture/Assets/Textures/TextureAsset.hpp"
#include <fstream>
#include <thread>
#include <chrono>
#include "Mixture/Util/FileSystemWatcher.hpp"

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

TEST_F(AssetRegistryTests, ConcurrentRegistrationAndReadsAreSafe)
{
    constexpr int threadCount = 8;
    constexpr int assetsPerThread = 64;
    std::array<std::vector<UUID>, threadCount> ids;
    std::vector<std::thread> workers;

    for (int threadIndex = 0; threadIndex < threadCount; ++threadIndex)
    {
        workers.emplace_back([threadIndex, &ids]() {
            for (int assetIndex = 0; assetIndex < assetsPerThread; ++assetIndex)
            {
                AssetMetadata metadata;
                metadata.ID = UUID(static_cast<uint64_t>(threadIndex * assetsPerThread + assetIndex + 1));
                metadata.Type = AssetType::Texture;
                metadata.FilePath = "Concurrent/" + std::to_string(threadIndex) + "/" + std::to_string(assetIndex);
                ids[threadIndex].push_back(metadata.ID);
                AssetRegistry::Get().RegisterAsset(metadata);
                EXPECT_TRUE(AssetRegistry::Get().Contains(metadata.ID));
                EXPECT_EQ(AssetRegistry::Get().GetPath(metadata.ID), metadata.FilePath);
            }
        });
    }
    for (auto& worker : workers) worker.join();

    EXPECT_EQ(AssetRegistry::Get().GetAssets().size(), threadCount * assetsPerThread);
    for (const auto& threadIDs : ids)
        for (UUID id : threadIDs)
            EXPECT_TRUE(AssetRegistry::Get().Contains(id));
}

TEST_F(AssetRegistryTests, NormalizedPathIndexSupportsLargeRegistries)
{
    constexpr uint64_t assetCount = 10000;
    for (uint64_t index = 1; index <= assetCount; ++index)
    {
        AssetMetadata metadata;
        metadata.ID = UUID(index);
        metadata.Type = AssetType::Shader;
        metadata.FilePath = "Generated/" + std::to_string(index) + ".spv";
        AssetRegistry::Get().RegisterAsset(metadata);
    }

    for (uint64_t index = 1; index <= assetCount; index += 97)
    {
        const auto metadata = AssetRegistry::Get().FindByPath(AssetType::Shader,
            "Generated/./" + std::to_string(index) + ".spv");
        EXPECT_EQ(metadata.ID, UUID(index));
    }
    EXPECT_FALSE(AssetRegistry::Get().FindByPath(AssetType::Texture, "Generated/1.spv").ID.IsValid());
}

TEST(FileSystemWatcherTests, LargeTreeScanTracksChangesWithoutSnapshotCopies)
{
    const std::filesystem::path root = std::filesystem::temp_directory_path()
        / ("MixtureWatcher-" + std::to_string(static_cast<uint64_t>(UUID())));
    std::filesystem::create_directories(root);
    constexpr size_t fileCount = 1500;
    for (size_t index = 0; index < fileCount; ++index)
    {
        std::ofstream stream(root / (std::to_string(index) + ".asset"));
        stream << index;
    }

    std::atomic<size_t> eventCount = 0;
    FileSystemWatcher watcher(root, [&eventCount](const std::filesystem::path&, FileAction) { ++eventCount; });
    ASSERT_EQ(watcher.GetTrackedFileCount(), fileCount);

    const auto start = std::chrono::steady_clock::now();
    watcher.ScanOnce();
    const auto elapsed = std::chrono::steady_clock::now() - start;
    EXPECT_EQ(eventCount, 0u);
    EXPECT_EQ(watcher.GetTrackedFileCount(), fileCount);
    EXPECT_LT(elapsed, std::chrono::seconds(5));

    std::filesystem::remove(root / "0.asset");
    {
        std::ofstream stream(root / "added.asset");
        stream << "added";
    }
    watcher.ScanOnce();
    EXPECT_EQ(eventCount, 2u);
    EXPECT_EQ(watcher.GetTrackedFileCount(), fileCount);
    std::filesystem::remove_all(root);
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

TEST_F(AssetManagerTests, LoadsAssetsOnOwnedExecutorAndWaitsForIdle)
{
    AssetManager& manager = AssetManager::Get();
    const std::filesystem::path root = std::filesystem::temp_directory_path()
        / ("MixtureAssetIO-" + std::to_string(static_cast<uint64_t>(UUID())));
    const std::filesystem::path shaderDirectory = root / "Shader";
    const std::filesystem::path shaderPath = shaderDirectory / "ExecutorTest.spv";

    std::filesystem::create_directories(shaderDirectory);
    {
        const std::array<char, 4> bytecode{ 0x03, 0x02, 0x23, 0x07 };
        std::ofstream stream(shaderPath, std::ios::binary);
        stream.write(bytecode.data(), static_cast<std::streamsize>(bytecode.size()));
    }

    manager.SetAssetRoot(root);
    const AssetHandle pending = manager.GetAsset(AssetType::Shader, shaderPath.filename());
    ASSERT_TRUE(pending.ID.IsValid());
    EXPECT_EQ(pending.Magic, 0u);

    manager.WaitForIdle();

    const AssetHandle loaded = manager.GetAsset(AssetType::Shader, shaderPath.filename());
    ASSERT_TRUE(loaded);
    Ref<ShaderAsset> shader = manager.GetResource<ShaderAsset>(loaded);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetBufferSize(), 4u);

    const std::filesystem::path cancelledPath = shaderDirectory / "Cancelled.spv";
    {
        std::ofstream stream(cancelledPath, std::ios::binary);
        stream.seekp((32 * 1024 * 1024) - 1);
        stream.put('\0');
    }

    const AssetHandle cancelled = manager.GetAsset(AssetType::Shader, cancelledPath.filename());
    ASSERT_TRUE(cancelled.ID.IsValid());
    EXPECT_TRUE(manager.CancelLoad(cancelled.ID));
    manager.WaitForIdle();
    EXPECT_FALSE(manager.IsAssetLoaded(cancelled.ID));

    manager.Shutdown();
    std::filesystem::remove_all(root);
}

TEST_F(AssetManagerTests, ConcurrentRequestsShareOneCacheEntry)
{
    AssetManager& manager = AssetManager::Get();
    const std::filesystem::path root = std::filesystem::temp_directory_path()
        / ("MixtureAssetCache-" + std::to_string(static_cast<uint64_t>(UUID())));
    const std::filesystem::path shaderDirectory = root / "Shader";
    const std::filesystem::path shaderPath = shaderDirectory / "Shared.spv";
    std::filesystem::create_directories(shaderDirectory);
    {
        const std::array<char, 4> bytecode{ 0x03, 0x02, 0x23, 0x07 };
        std::ofstream stream(shaderPath, std::ios::binary);
        stream.write(bytecode.data(), static_cast<std::streamsize>(bytecode.size()));
    }
    AssetMetadata metadata;
    metadata.ID = UUID();
    metadata.Type = AssetType::Shader;
    metadata.FilePath = shaderPath;
    AssetSerializer::WriteMetadata(metadata);

    manager.SetAssetRoot(root);
    std::array<AssetHandle, 16> handles;
    std::vector<std::thread> workers;
    for (auto& handle : handles)
        workers.emplace_back([&manager, &handle, &shaderPath]() {
            handle = manager.GetAsset(AssetType::Shader, shaderPath.filename());
        });
    for (auto& worker : workers) worker.join();

    for (const AssetHandle handle : handles)
        EXPECT_EQ(handle.ID, metadata.ID);
    manager.WaitForIdle();
    EXPECT_TRUE(manager.IsAssetLoaded(metadata.ID));

    manager.Shutdown();
    std::filesystem::remove_all(root);
}

TEST_F(AssetManagerTests, SteadyStateLookupAvoidsMetadataFileAccess)
{
    AssetManager& manager = AssetManager::Get();
    const std::filesystem::path root = std::filesystem::temp_directory_path()
        / ("MixtureAssetLookup-" + std::to_string(static_cast<uint64_t>(UUID())));
    const std::filesystem::path shaderDirectory = root / "Shader";
    const std::filesystem::path shaderPath = shaderDirectory / "Cached.spv";
    std::filesystem::create_directories(shaderDirectory);
    {
        const std::array<char, 4> bytecode{ 0x03, 0x02, 0x23, 0x07 };
        std::ofstream stream(shaderPath, std::ios::binary);
        stream.write(bytecode.data(), static_cast<std::streamsize>(bytecode.size()));
    }

    manager.SetAssetRoot(root);
    manager.GetAsset(AssetType::Shader, shaderPath.filename());
    manager.WaitForIdle();
    const uint64_t coldPathAccesses = manager.GetMetadataFileAccessCount();
    ASSERT_GT(coldPathAccesses, 0u);

    for (int frame = 0; frame < 1000; ++frame)
        EXPECT_TRUE(manager.GetAsset(AssetType::Shader, shaderPath.filename()));
    EXPECT_EQ(manager.GetMetadataFileAccessCount(), coldPathAccesses);

    manager.Shutdown();
    std::filesystem::remove_all(root);
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

TEST_F(AssetManagerTests, ShaderCompilerProducesValidOutputWhenAvailable)
{
    if (!ShaderCompiler::IsAvailable())
        GTEST_SKIP() << "DXC runtime is unavailable";

    AssetManager::Get().SetGraphicsAPI(RHI::GraphicsAPI::Vulkan);
    std::string source = R"(
        [shader("vertex")]
        float4 main(float4 position : POSITION) : SV_Position { return position; }
    )";

    const auto spirv = ShaderCompiler::Compile(source);
    ASSERT_FALSE(spirv.empty());
    ASSERT_EQ(spirv.size() % sizeof(uint32_t), 0u);
    uint32_t magic = 0;
    std::memcpy(&magic, spirv.data(), sizeof(magic));
    EXPECT_EQ(magic, 0x07230203u);

    const auto reflection = ShaderCompiler::ReflectSPIRV(spirv.data(), spirv.size());
    ASSERT_TRUE(reflection.EntryPoints.contains(RHI::ShaderStage::Vertex));
    EXPECT_EQ(reflection.EntryPoints.at(RHI::ShaderStage::Vertex), "main");
}

TEST_F(AssetManagerTests, ShaderCompilerPropagatesFailure)
{
    const auto result = ShaderCompiler::Compile("this is not valid HLSL");
    EXPECT_TRUE(result.empty());
}
