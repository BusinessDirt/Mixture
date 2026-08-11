#include <gtest/gtest.h>
#include "Mixture/Assets/AssetRegistry.hpp"
#include "Mixture/Assets/AssetManager.hpp"
#include "Mixture/Assets/AssetSerializer.hpp"
#include "Mixture/Assets/Shaders/ShaderAsset.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"
#include "Mixture/Assets/Shaders/IShaderReflector.hpp"
#include "Mixture/Assets/Textures/TextureAsset.hpp"
#include "Mixture/Util/FileStreamReader.hpp"
#include <fstream>
#include <thread>
#include <chrono>
#include <mutex>
#include <unordered_set>
#include "Mixture/Util/FileSystemWatcher.hpp"

using namespace Mixture;

namespace
{
    bool IsPathWithin(const std::filesystem::path& root, const std::filesystem::path& candidate)
    {
        auto rootIt = root.begin();
        auto candidateIt = candidate.begin();
        for (; rootIt != root.end(); ++rootIt, ++candidateIt)
        {
            if (candidateIt == candidate.end() || *candidateIt != *rootIt) return false;
        }
        return true;
    }

    class TestFileSystemAssetResolver final : public IAssetFileResolver
    {
    public:
        explicit TestFileSystemAssetResolver(std::filesystem::path root) : m_Root(std::move(root)) {}

        std::optional<std::filesystem::path> Resolve(
            AssetType type, const std::filesystem::path& path) const override
        {
            if (path.empty() || path.is_absolute() || type <= AssetType::None || type >= AssetType::Count)
                return std::nullopt;

            std::error_code error;
            const auto typeRoot = std::filesystem::weakly_canonical(
                m_Root / Utils::AssetTypeToString(type), error);
            if (error) return std::nullopt;

            const auto candidate = std::filesystem::weakly_canonical(typeRoot / path, error);
            if (error || !IsPathWithin(typeRoot, candidate)) return std::nullopt;
            return candidate;
        }

    private:
        std::filesystem::path m_Root;
    };

    void ConfigureTestAssetRoot(AssetManager& manager, const std::filesystem::path& root)
    {
        manager.SetAssetRoot(root);
        manager.SetAssetFileResolver(CreateRef<TestFileSystemAssetResolver>(root));
    }
}

TEST(UUIDTests, ConcurrentGenerationIsSafeAndProducesValidValues)
{
    constexpr size_t threadCount = 8;
    constexpr size_t idsPerThread = 1000;
    std::unordered_set<uint64_t> values;
    std::mutex valuesMutex;
    std::vector<std::thread> workers;

    for (size_t thread = 0; thread < threadCount; ++thread)
    {
        workers.emplace_back([&]() {
            std::vector<uint64_t> localValues;
            localValues.reserve(idsPerThread);
            for (size_t index = 0; index < idsPerThread; ++index)
            {
                UUID id;
                EXPECT_TRUE(id.IsValid());
                localValues.push_back(static_cast<uint64_t>(id));
            }
            std::lock_guard<std::mutex> lock(valuesMutex);
            values.insert(localValues.begin(), localValues.end());
        });
    }
    for (auto& worker : workers) worker.join();

    EXPECT_EQ(values.size(), threadCount * idsPerThread);
}

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

TEST_F(AssetRegistryTests, RejectsConflictingGUIDsAndPaths)
{
    const AssetMetadata original{ UUID(101), AssetType::Texture, "Original.png" };
    EXPECT_TRUE(AssetRegistry::Get().RegisterAsset(original));
    EXPECT_TRUE(AssetRegistry::Get().RegisterAsset(original));

    EXPECT_FALSE(AssetRegistry::Get().RegisterAsset({ UUID(101), AssetType::Shader, "Other.spv" }));
    EXPECT_FALSE(AssetRegistry::Get().RegisterAsset({ UUID(202), AssetType::Texture, "Original.png" }));
    EXPECT_EQ(AssetRegistry::Get().GetMetadata(UUID(101)).FilePath, original.FilePath);
    EXPECT_FALSE(AssetRegistry::Get().Contains(UUID(202)));
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

TEST(AssetFileResolverTests, DefaultResolverPreservesTypedRootAndRejectsEscapes)
{
    const std::filesystem::path root = std::filesystem::temp_directory_path()
        / ("MixtureAssetStrategy-" + std::to_string(static_cast<uint64_t>(UUID())));
    std::filesystem::create_directories(root / "Shader");

    const auto resolver = CreateRef<TestFileSystemAssetResolver>(root);
    const auto resolved = resolver->Resolve(AssetType::Shader, "Nested/Test.spv");
    ASSERT_TRUE(resolved.has_value());
    EXPECT_EQ(*resolved, std::filesystem::weakly_canonical(root / "Shader/Nested/Test.spv"));
    EXPECT_FALSE(resolver->Resolve(AssetType::Shader, "../../Outside.spv").has_value());
    EXPECT_FALSE(resolver->Resolve(AssetType::Shader, root / "Shader/Test.spv").has_value());

    std::filesystem::remove_all(root);
}

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

TEST_F(AssetManagerTests, DelegatesPathResolutionToConfiguredResolver)
{
    class RejectingResolver final : public IAssetFileResolver
    {
    public:
        std::optional<std::filesystem::path> Resolve(
            AssetType, const std::filesystem::path&) const override
        {
            ++Calls;
            return std::nullopt;
        }

        mutable size_t Calls = 0;
    };

    auto resolver = CreateRef<RejectingResolver>();
    AssetManager::Get().SetAssetFileResolver(resolver);

    EXPECT_FALSE(AssetManager::Get().GetAsset(AssetType::Shader, "Test.spv"));
    EXPECT_EQ(resolver->Calls, 1u);
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

    ConfigureTestAssetRoot(manager, root);
    const AssetHandle pending = manager.GetAsset(AssetType::Shader, shaderPath.filename());
    ASSERT_TRUE(pending.ID.IsValid());
    EXPECT_EQ(pending.Magic, 0u);

    manager.WaitForIdle();

    const AssetHandle loaded = manager.GetAsset(AssetType::Shader, shaderPath.filename());
    ASSERT_TRUE(loaded);
    Ref<ShaderAsset> shader = manager.GetResource<ShaderAsset>(loaded);
    ASSERT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetBufferSize(), 4u);
    EXPECT_EQ(manager.GetResource<TextureAsset>(loaded), nullptr);

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

TEST_F(AssetManagerTests, ShutdownJoinsActiveReadsBeforeReturning)
{
    AssetManager& manager = AssetManager::Get();
    const std::filesystem::path root = std::filesystem::temp_directory_path()
        / ("MixtureAssetShutdown-" + std::to_string(static_cast<uint64_t>(UUID())));
    const std::filesystem::path shaderDirectory = root / "Shader";
    const std::filesystem::path shaderPath = shaderDirectory / "Large.spv";
    std::filesystem::create_directories(shaderDirectory);
    {
        std::ofstream stream(shaderPath, std::ios::binary);
        stream.seekp((32 * 1024 * 1024) - 1);
        stream.put('\0');
    }

    std::atomic<size_t> callbackCount = 0;
    manager.AddReloadCallback([&callbackCount](AssetType, UUID) { ++callbackCount; });
    ConfigureTestAssetRoot(manager, root);
    const AssetHandle pending = manager.GetAsset(AssetType::Shader, shaderPath.filename());
    ASSERT_TRUE(pending.ID.IsValid());

    manager.Shutdown();
    const size_t callbacksAtShutdown = callbackCount.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    EXPECT_FALSE(manager.IsInitialized());
    EXPECT_FALSE(manager.IsAssetLoaded(pending.ID));
    EXPECT_EQ(callbackCount.load(), callbacksAtShutdown);
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

    ConfigureTestAssetRoot(manager, root);
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

TEST_F(AssetManagerTests, RejectsPathsOutsideAssetTypeRoot)
{
    AssetManager& manager = AssetManager::Get();
    const std::filesystem::path root = std::filesystem::temp_directory_path()
        / ("MixtureAssetBoundary-" + std::to_string(static_cast<uint64_t>(UUID())));
    const std::filesystem::path outside = root.parent_path() / (root.filename().string() + "-outside.spv");
    std::filesystem::create_directories(root / "Shader");
    {
        std::ofstream stream(outside, std::ios::binary);
        stream << "outside";
    }

    ConfigureTestAssetRoot(manager, root);
    EXPECT_FALSE(manager.GetAsset(AssetType::Shader, outside));
    EXPECT_FALSE(manager.GetAsset(AssetType::Shader, "../../" + outside.filename().string()));

    manager.Shutdown();
    std::filesystem::remove_all(root);
    std::filesystem::remove(outside);
}

TEST_F(AssetManagerTests, RejectsSymlinkEscapesWhenSupported)
{
    AssetManager& manager = AssetManager::Get();
    const std::filesystem::path root = std::filesystem::temp_directory_path()
        / ("MixtureAssetSymlink-" + std::to_string(static_cast<uint64_t>(UUID())));
    const std::filesystem::path outside = root.parent_path() / (root.filename().string() + "-outside.spv");
    const std::filesystem::path link = root / "Shader" / "Escaped.spv";
    std::filesystem::create_directories(root / "Shader");
    {
        std::ofstream stream(outside, std::ios::binary);
        stream << "outside";
    }
    std::error_code error;
    std::filesystem::create_symlink(outside, link, error);
    if (error)
    {
        std::filesystem::remove_all(root);
        std::filesystem::remove(outside);
        GTEST_SKIP() << "Symbolic links are unavailable: " << error.message();
    }

    ConfigureTestAssetRoot(manager, root);
    EXPECT_FALSE(manager.GetAsset(AssetType::Shader, link.filename()));

    manager.Shutdown();
    std::filesystem::remove_all(root);
    std::filesystem::remove(outside);
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

    ConfigureTestAssetRoot(manager, root);
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

TEST(AssetSerializerTests, RejectsMalformedMissingAndOutOfRangeMetadata)
{
    const std::filesystem::path assetPath = std::filesystem::temp_directory_path()
        / ("MixtureInvalidMetadata-" + std::to_string(static_cast<uint64_t>(UUID())));
    const std::filesystem::path metadataPath = assetPath.string() + ".meta";
    AssetMetadata metadata;

    {
        std::ofstream stream(metadataPath);
        stream << "GUID=not-a-number\nType=1\n";
    }
    EXPECT_FALSE(AssetSerializer::TryLoadMetadata(assetPath, metadata));

    {
        std::ofstream stream(metadataPath);
        stream << "GUID=42\n";
    }
    EXPECT_FALSE(AssetSerializer::TryLoadMetadata(assetPath, metadata));

    {
        std::ofstream stream(metadataPath);
        stream << "GUID=42\nType=255\n";
    }
    EXPECT_FALSE(AssetSerializer::TryLoadMetadata(assetPath, metadata));

    {
        std::ofstream stream(metadataPath);
        stream << "GUID=42\nGUID=43\nType=1\n";
    }
    EXPECT_FALSE(AssetSerializer::TryLoadMetadata(assetPath, metadata));
    std::filesystem::remove(metadataPath);
}

TEST(FileStreamReaderTests, ReportsCompleteAndFailedReads)
{
    const std::filesystem::path path = std::filesystem::temp_directory_path()
        / ("MixtureFileReader-" + std::to_string(static_cast<uint64_t>(UUID())));
    {
        std::ofstream stream(path, std::ios::binary);
        stream << "abcd";
    }

    Vector<char> data;
    {
        FileStreamReader reader(path);
        EXPECT_EQ(reader.GetFileSize(), 4u);
        EXPECT_TRUE(reader.ReadBuffer(data));
        EXPECT_EQ(data, (Vector<char>{ 'a', 'b', 'c', 'd' }));

        FileStreamReader missing(path.string() + ".missing");
        EXPECT_EQ(missing.GetFileSize(), 0u);
        EXPECT_FALSE(missing.ReadBuffer(data));
    }
    std::filesystem::remove(path);
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
        GTEST_SKIP() << "Slang runtime is unavailable";

    AssetManager::Get().SetGraphicsAPI(RHI::GraphicsAPI::Vulkan);
    std::string source = R"(
        [shader("vertex")]
        float4 main(float4 position : POSITION) : SV_Position { return position; }
    )";

    const auto result = ShaderCompiler::CompileDetailed(source);
    ASSERT_TRUE(result.Succeeded()) << result.Diagnostics;
    const auto& spirv = result.Bytecode;
    ASSERT_EQ(spirv.size() % sizeof(uint32_t), 0u);
    uint32_t magic = 0;
    std::memcpy(&magic, spirv.data(), sizeof(magic));
    EXPECT_EQ(magic, 0x07230203u);

    const auto reflector = IShaderReflector::Create(RHI::GraphicsAPI::Vulkan);
    ASSERT_NE(reflector, nullptr);
    const auto reflection = reflector->Reflect(spirv.data(), spirv.size());
    ASSERT_TRUE(reflection.EntryPoints.contains(RHI::ShaderStage::Vertex));
    EXPECT_EQ(reflection.EntryPoints.at(RHI::ShaderStage::Vertex), "main");
}

TEST_F(AssetManagerTests, ShaderCompilerProducesVulkanAndD3D12Bytecode)
{
    if (!ShaderCompiler::IsAvailable())
        GTEST_SKIP() << "Slang runtime is unavailable";

    const std::string source = R"(
        [shader("compute")]
        [numthreads(1, 1, 1)]
        void main(uint3 dispatchThreadID : SV_DispatchThreadID) {}
    )";

    for (const RHI::GraphicsAPI api : {
             RHI::GraphicsAPI::Vulkan,
             RHI::GraphicsAPI::D3D12,
         })
    {
        const auto result = ShaderCompiler::CompileDetailed(source, api);
        ASSERT_TRUE(result.Succeeded()) << "API " << static_cast<int>(api) << ": " << result.Diagnostics;
        EXPECT_FALSE(result.Bytecode.empty());
    }
}

TEST_F(AssetManagerTests, ShaderCompilerReportsMetalTargetGenerationFailures)
{
    if (!ShaderCompiler::IsAvailable())
        GTEST_SKIP() << "Slang runtime is unavailable";

    const auto result = ShaderCompiler::CompileDetailed(R"(
        [shader("compute")]
        [numthreads(1, 1, 1)]
        void main(uint3 dispatchThreadID : SV_DispatchThreadID) {}
    )", RHI::GraphicsAPI::Metal);

    // Generating a metallib requires Slang's downstream Metal compiler. The
    // result is valid in both environments: bytecode when it is installed,
    // otherwise a usable diagnostic without partial output.
    if (result.Succeeded())
    {
        EXPECT_FALSE(result.Bytecode.empty());
    }
    else
    {
        EXPECT_TRUE(result.Bytecode.empty());
        EXPECT_FALSE(result.Diagnostics.empty());
    }
}

TEST(ShaderCompilerTests, RejectsCompilationWithoutATargetAPI)
{
    const auto result = ShaderCompiler::CompileDetailed("[shader(\"compute\")] void main() {}", RHI::GraphicsAPI::None);
    EXPECT_FALSE(result.Succeeded());
    EXPECT_TRUE(result.Bytecode.empty());
    EXPECT_EQ(result.Diagnostics, "No valid RHI::GraphicsAPI was selected");
}

TEST(ShaderReflectorTests, CreatesReflectorsForEverySupportedAPI)
{
    EXPECT_NE(IShaderReflector::Create(RHI::GraphicsAPI::Vulkan), nullptr);
    EXPECT_NE(IShaderReflector::Create(RHI::GraphicsAPI::D3D12), nullptr);
    EXPECT_NE(IShaderReflector::Create(RHI::GraphicsAPI::Metal), nullptr);
    EXPECT_EQ(IShaderReflector::Create(RHI::GraphicsAPI::None), nullptr);
}

TEST(ShaderCompilerTests, RejectsMalformedSPIRVWithoutDereferencingIt)
{
    const Vector<uint8_t> truncated{ 0x03, 0x02, 0x23 };
    const auto reflector = IShaderReflector::Create(RHI::GraphicsAPI::Vulkan);
    ASSERT_NE(reflector, nullptr);
    EXPECT_TRUE(reflector->Reflect(truncated.data(), truncated.size()).EntryPoints.empty());

    alignas(uint32_t) const std::array<uint32_t, 2> wrongMagic{ 0xDEADBEEF, 0 };
    EXPECT_TRUE(reflector->Reflect(wrongMagic.data(), sizeof(wrongMagic)).EntryPoints.empty());
    EXPECT_EQ(IShaderReflector::Create(RHI::GraphicsAPI::None), nullptr);
}

TEST_F(AssetManagerTests, ShaderCompilerPropagatesFailure)
{
    const auto result = ShaderCompiler::CompileDetailed("this is not valid HLSL");
    EXPECT_FALSE(result.Succeeded());
    EXPECT_TRUE(result.Bytecode.empty());
    EXPECT_FALSE(result.Diagnostics.empty());
}
