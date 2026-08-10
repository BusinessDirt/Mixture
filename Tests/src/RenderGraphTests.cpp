#include <gtest/gtest.h>

#include "Mixture/Render/Graph/RenderGraph.hpp"
#include "Mixture/Render/Graph/RenderGraphDefinitions.hpp"
#include "Mixture/Render/Graph/RenderGraphResourceCache.hpp"
#include "Mixture/Render/Graph/RenderGraphRegistry.hpp"
#include "Mixture/Render/PipelineCache.hpp"
#include "Mixture/Render/ShaderLibrary.hpp"
#include "Mixture/Render/RHI/IGraphicsContext.hpp"
#include "Mixture/Assets/AssetManager.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Pipeline/Pipeline.hpp"
#include "Platform/Vulkan/Pipeline/Shader.hpp"
#include "Platform/Vulkan/Resources/Buffer.hpp"
#include "Platform/Vulkan/Resources/Texture.hpp"
#include "Platform/Vulkan/Resources/AllocationPolicy.hpp"
#include "Platform/Vulkan/SingleTimeCommand.hpp"
#include "Platform/Vulkan/FrameSubmission.hpp"
#include "Platform/Vulkan/ResourcePolicy.hpp"
#include "Platform/Vulkan/Descriptors/Builder.hpp"
#include "Platform/Vulkan/Instance.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"

#include <filesystem>
#include <fstream>
#include <type_traits>
#include <chrono>

namespace Mixture::Tests
{
    namespace
    {
        RGPassNode MakePass(const char* name)
        {
            RGPassNode pass;
            pass.Name = name;
            return pass;
        }

        RGAttachmentInfo AttachmentWrite(RGResourceHandle handle)
        {
            RGAttachmentInfo write;
            write.Handle = handle;
            return write;
        }

        void ExpectOrder(const Vector<RGPassNode>& passes, std::initializer_list<const char*> expected)
        {
            ASSERT_EQ(passes.size(), expected.size());

            size_t index = 0;
            for (const char* name : expected)
            {
                EXPECT_EQ(passes[index].Name, name);
                ++index;
            }
        }

        class MockTexture final : public RHI::ITexture
        {
        public:
            explicit MockTexture(const RHI::TextureDesc& desc)
                : m_Desc(desc)
            {}

            uint32_t GetWidth() const override { return m_Desc.Width; }
            uint32_t GetHeight() const override { return m_Desc.Height; }
            RHI::Format GetFormat() const override { return m_Desc.PixelFormat; }
            std::string_view GetDebugName() const override { return m_Desc.DebugName; }

        private:
            RHI::TextureDesc m_Desc;
        };

        class MockBuffer final : public RHI::IBuffer
        {
        public:
            explicit MockBuffer(const RHI::BufferDesc& desc)
                : m_Desc(desc)
            {}

            uint64_t GetSize() const override { return m_Desc.Size; }
            RHI::BufferUsage GetUsage() const override { return m_Desc.Usage; }

        private:
            RHI::BufferDesc m_Desc;
        };

        class MockShader final : public RHI::IShader
        {
        public:
            explicit MockShader(RHI::ShaderIdentity identity)
                : m_Identity(identity)
            {}

            RHI::ShaderIdentity GetIdentity() const override { return m_Identity; }
            RHI::ShaderStage GetStage() const override { return m_Identity.Stage; }

        private:
            RHI::ShaderIdentity m_Identity;
        };

        class MockPipeline final : public RHI::IPipeline
        {
        public:
            explicit MockPipeline(size_t& destructionCount)
                : m_DestructionCount(destructionCount)
            {}

            ~MockPipeline() override { ++m_DestructionCount; }

        private:
            size_t& m_DestructionCount;
        };

        class MockGraphicsDevice final : public RHI::IGraphicsDevice
        {
        public:
            Ref<RHI::IShader> CreateShader(const void*, size_t, RHI::ShaderStage,
                RHI::ShaderIdentity identity) override
            {
                return CreateRef<MockShader>(identity);
            }

            Ref<RHI::IBuffer> CreateBuffer(const RHI::BufferDesc& desc, const void*) override
            {
                ++BufferCreationCount;
                return CreateRef<MockBuffer>(desc);
            }

            Ref<RHI::ITexture> CreateTexture(const RHI::TextureDesc& desc, const void*) override
            {
                ++TextureCreationCount;
                return CreateRef<MockTexture>(desc);
            }

            Ref<RHI::IPipeline> CreatePipeline(const RHI::PipelineDesc&) override
            {
                ++PipelineCreationCount;
                return CreateRef<MockPipeline>(PipelineDestructionCount);
            }

            void WaitForIdle() override {}

            size_t BufferCreationCount = 0;
            size_t TextureCreationCount = 0;
            size_t PipelineCreationCount = 0;
            size_t PipelineDestructionCount = 0;
        };

        class HeadlessGraphicsContext final : public RHI::IGraphicsContext
        {
        public:
            RHI::GraphicsAPI GetAPI() const override { return RHI::GraphicsAPI::None; }
            RHI::IGraphicsDevice& GetDevice() const override { return m_Device; }
            void OnResize(uint32_t, uint32_t) override {}
            RHI::ITexture* BeginFrame() override { return nullptr; }
            void EndFrame() override {}
            Scope<RHI::ICommandList> GetCommandBuffer() override { return nullptr; }
            uint32_t GetSwapchainWidth() const override { return 0; }
            uint32_t GetSwapchainHeight() const override { return 0; }
            uint32_t GetCurrentFrameIndex() const override { return 0; }

        private:
            mutable MockGraphicsDevice m_Device;
        };

        template<typename T>
        concept HasImGuiLifecycle = requires(T& context, RHI::ICommandList* commandList)
        {
            context.BeginImGuiFrame();
            context.EndImGuiFrame();
            context.RenderImGui(commandList);
        };
    }

    TEST(RenderGraphTests, PreservesIndependentPassOrder)
    {
        Vector<RGPassNode> passes;
        passes.push_back(MakePass("First"));
        passes.push_back(MakePass("Second"));
        passes.push_back(MakePass("Third"));

        ASSERT_TRUE(RenderGraphAlgorithms::SortPasses(passes));
        ExpectOrder(passes, { "First", "Second", "Third" });
    }

    TEST(VulkanSubmissionTests, SkipsEveryIdleQueueCombination)
    {
        for (int mask = 0; mask < 4; ++mask)
        {
            Vulkan::FrameQueueActivity activity;
            activity.Graphics = true;
            activity.Transfer = (mask & 1) != 0;
            activity.Compute = (mask & 2) != 0;

            const auto plan = Vulkan::BuildFrameSubmissionPlan(activity);
            EXPECT_EQ(plan.SubmitTransfer, activity.Transfer);
            EXPECT_EQ(plan.SubmitCompute, activity.Compute);
            EXPECT_EQ(plan.WaitForTransfer, activity.Transfer);
            EXPECT_EQ(plan.WaitForCompute, activity.Compute);
        }
    }

    TEST(VulkanDescriptorTests, BuilderOwnsCopiedDescriptorInformation)
    {
        static_assert(std::is_same_v<decltype(std::declval<Vulkan::DescriptorBuilder&>().BindBuffer(
            0, std::declval<const vk::DescriptorBufferInfo&>())), Vulkan::DescriptorBuilder&>);
        static_assert(std::is_same_v<decltype(std::declval<Vulkan::DescriptorBuilder&>().BindImage(
            0, std::declval<const vk::DescriptorImageInfo&>())), Vulkan::DescriptorBuilder&>);
        SUCCEED();
    }

    TEST(VulkanPipelineLayoutTests, MergesGraphicsAndComputeReflectionBySetAndBinding)
    {
        ShaderReflectionData vertex;
        vertex.UniformBuffers.push_back({ "Globals", ShaderReflectionData::ResourceType::UniformBuffer, 0, 0, 64, 1 });
        vertex.PushConstants.push_back({ "Draw", 0, 16, 0 });

        ShaderReflectionData fragment;
        fragment.UniformBuffers.push_back({ "Globals", ShaderReflectionData::ResourceType::UniformBuffer, 0, 0, 64, 1 });
        fragment.Textures.push_back({ "Albedo", ShaderReflectionData::ResourceType::SampledImage, 1, 2, 0, 1 });

        ShaderReflectionData compute;
        compute.StorageImages.push_back({ "Output", ShaderReflectionData::ResourceType::StorageImage, 2, 1, 0, 1 });

        const auto layout = Vulkan::BuildPipelineLayoutDescription({
            { &vertex, RHI::ShaderStage::Vertex },
            { &fragment, RHI::ShaderStage::Fragment },
            { &compute, RHI::ShaderStage::Compute }
        });

        ASSERT_EQ(layout.Sets.size(), 3u);
        ASSERT_EQ(layout.Sets[0].size(), 1u);
        EXPECT_EQ(layout.Sets[0][0].stageFlags,
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
        EXPECT_EQ(layout.Sets[1][0].descriptorType, vk::DescriptorType::eCombinedImageSampler);
        EXPECT_EQ(layout.Sets[2][0].descriptorType, vk::DescriptorType::eStorageImage);
        ASSERT_EQ(layout.PushConstants.size(), 1u);
        EXPECT_EQ(layout.PushConstants[0].size, 16u);
    }

    TEST(VulkanSynchronizationTests, MapsEveryResourceStateToConcreteSynchronization)
    {
        const auto renderTarget = Vulkan::MapResourceState(RHI::ResourceState::RenderTarget);
        EXPECT_EQ(renderTarget.Layout, vk::ImageLayout::eColorAttachmentOptimal);
        EXPECT_TRUE(static_cast<bool>(renderTarget.Access & vk::AccessFlagBits::eColorAttachmentWrite));

        const auto copyDestination = Vulkan::MapResourceState(RHI::ResourceState::CopyDest);
        EXPECT_EQ(copyDestination.Layout, vk::ImageLayout::eTransferDstOptimal);
        EXPECT_EQ(copyDestination.Stages, vk::PipelineStageFlagBits::eTransfer);

        const auto shaderResource = Vulkan::MapResourceState(RHI::ResourceState::ShaderResource);
        EXPECT_EQ(shaderResource.Layout, vk::ImageLayout::eShaderReadOnlyOptimal);
        EXPECT_TRUE(static_cast<bool>(shaderResource.Access & vk::AccessFlagBits::eShaderRead));
    }

    TEST(VulkanQueueTests, DeduplicatesDistinctQueueFamilyConfigurations)
    {
        Vulkan::QueueFamilyIndices indices;
        indices.Graphics = 3;
        indices.Present = 7;
        indices.Transfer = 5;
        indices.Compute = 3;
        EXPECT_EQ(Vulkan::CollectQueueFamilyIndices(indices), (Vector<uint32_t>{ 3, 5, 7 }));
    }

    TEST(VulkanCapabilityTests, RejectsMissingRequiredCapabilities)
    {
        Vector<vk::ExtensionProperties> noExtensions;
        EXPECT_FALSE(Vulkan::Instance::HasRequiredExtensions(noExtensions, { VK_KHR_SURFACE_EXTENSION_NAME }));
        EXPECT_FALSE(Vulkan::PhysicalDevice::HasRequiredExtensions(noExtensions));
        EXPECT_FALSE(Vulkan::PhysicalDevice::HasRequiredFeatures(true, false, true));
        EXPECT_FALSE(Vulkan::PhysicalDevice::HasUsableSurface({}, {}));

        vk::ExtensionProperties swapchainExtension;
        std::strncpy(swapchainExtension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE - 1);
        EXPECT_TRUE(Vulkan::PhysicalDevice::HasRequiredExtensions({ swapchainExtension }));
        EXPECT_TRUE(Vulkan::PhysicalDevice::HasRequiredFeatures(true, true, true));
        EXPECT_TRUE(Vulkan::PhysicalDevice::HasUsableSurface(
            { vk::SurfaceFormatKHR{} }, { vk::PresentModeKHR::eFifo }));
    }

    TEST(VulkanTexturePolicyTests, SupportsAllUsageAndAspectClasses)
    {
        const auto usage = Vulkan::MapTextureUsage(
            RHI::TextureUsage::Sampled | RHI::TextureUsage::Storage |
            RHI::TextureUsage::ColorAttachment | RHI::TextureUsage::DepthStencilAttachment |
            RHI::TextureUsage::TransferSource | RHI::TextureUsage::TransferDestination);
        EXPECT_TRUE(static_cast<bool>(usage & vk::ImageUsageFlagBits::eSampled));
        EXPECT_TRUE(static_cast<bool>(usage & vk::ImageUsageFlagBits::eStorage));
        EXPECT_TRUE(static_cast<bool>(usage & vk::ImageUsageFlagBits::eColorAttachment));
        EXPECT_TRUE(static_cast<bool>(usage & vk::ImageUsageFlagBits::eDepthStencilAttachment));
        EXPECT_TRUE(static_cast<bool>(usage & vk::ImageUsageFlagBits::eTransferSrc));
        EXPECT_TRUE(static_cast<bool>(usage & vk::ImageUsageFlagBits::eTransferDst));
        EXPECT_EQ(Vulkan::GetImageAspect(RHI::Format::R8G8B8A8_UNORM), vk::ImageAspectFlagBits::eColor);
        EXPECT_EQ(Vulkan::GetImageAspect(RHI::Format::D32_FLOAT), vk::ImageAspectFlagBits::eDepth);
        EXPECT_EQ(Vulkan::GetImageAspect(RHI::Format::D24_UNORM_S8_UINT),
            vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
    }

    TEST(RenderGraphTests, IndexedLookupScalesAcrossRepresentativeGraphSizes)
    {
        MockGraphicsDevice device;
        RenderGraph graph(device);
        RHI::TextureDesc desc;

        constexpr size_t ResourceCount = 10000;
        for (size_t index = 0; index < ResourceCount; ++index)
            graph.CreateResource("Resource-" + std::to_string(index), desc);

        const auto start = std::chrono::steady_clock::now();
        for (size_t index = 0; index < ResourceCount; ++index)
            EXPECT_EQ(graph.GetResource("Resource-" + std::to_string(index)).ID, index);
        const auto elapsed = std::chrono::steady_clock::now() - start;

        EXPECT_LT(elapsed, std::chrono::seconds(2));
    }

    TEST(RenderGraphTests, PreservesReadAfterWriteDependency)
    {
        const RGResourceHandle resource{ 0 };
        Vector<RGPassNode> passes;
        passes.push_back(MakePass("Writer"));
        passes.back().Writes.push_back(AttachmentWrite(resource));
        passes.push_back(MakePass("Reader"));
        passes.back().Reads.push_back(resource);

        ASSERT_TRUE(RenderGraphAlgorithms::SortPasses(passes));
        ExpectOrder(passes, { "Writer", "Reader" });
    }

    TEST(RenderGraphTests, PreservesAllReadersBeforeALaterWriter)
    {
        const RGResourceHandle resource{ 0 };
        Vector<RGPassNode> passes;
        passes.push_back(MakePass("First reader"));
        passes.back().Reads.push_back(resource);
        passes.push_back(MakePass("Second reader"));
        passes.back().Reads.push_back(resource);
        passes.push_back(MakePass("Writer"));
        passes.back().BufferWrites.push_back(resource);

        ASSERT_TRUE(RenderGraphAlgorithms::SortPasses(passes));
        ExpectOrder(passes, { "First reader", "Second reader", "Writer" });
    }

    TEST(RenderGraphTests, PreservesWriteAfterWriteDependency)
    {
        const RGResourceHandle resource{ 0 };
        Vector<RGPassNode> passes;
        passes.push_back(MakePass("First writer"));
        passes.back().Writes.push_back(AttachmentWrite(resource));
        passes.push_back(MakePass("Second writer"));
        passes.back().Writes.push_back(AttachmentWrite(resource));

        ASSERT_TRUE(RenderGraphAlgorithms::SortPasses(passes));
        ExpectOrder(passes, { "First writer", "Second writer" });
    }

    TEST(RenderGraphTests, DeduplicatesRepeatedAndReadWriteAccesses)
    {
        const RGResourceHandle resource{ 0 };
        Vector<RGPassNode> passes;
        passes.push_back(MakePass("Producer"));
        passes.back().BufferWrites.push_back(resource);
        passes.push_back(MakePass("Read modifier"));
        passes.back().Reads.push_back(resource);
        passes.back().Reads.push_back(resource);
        passes.back().BufferWrites.push_back(resource);
        passes.push_back(MakePass("Consumer"));
        passes.back().Reads.push_back(resource);

        ASSERT_TRUE(RenderGraphAlgorithms::SortPasses(passes));
        ExpectOrder(passes, { "Producer", "Read modifier", "Consumer" });
    }

    TEST(RenderGraphTests, CullsPassesWhoseTransientOutputsAreUnused)
    {
        const RGResourceHandle liveTransient{ 0 };
        const RGResourceHandle importedOutput{ 1 };
        const RGResourceHandle deadTransient{ 2 };

        Vector<RGResourceNode> resources(3);
        resources[0].Handle = liveTransient;
        resources[0].Type = RGResourceType::Texture;
        resources[1].Handle = importedOutput;
        resources[1].Type = RGResourceType::ImportedTexture;
        resources[2].Handle = deadTransient;
        resources[2].Type = RGResourceType::Texture;

        Vector<RGPassNode> passes;
        passes.push_back(MakePass("Dead producer"));
        passes.back().Writes.push_back(AttachmentWrite(deadTransient));
        passes.push_back(MakePass("Live producer"));
        passes.back().Writes.push_back(AttachmentWrite(liveTransient));
        passes.push_back(MakePass("Output"));
        passes.back().Reads.push_back(liveTransient);
        passes.back().Writes.push_back(AttachmentWrite(importedOutput));

        RenderGraphAlgorithms::CullPasses(passes, resources);

        ExpectOrder(passes, { "Live producer", "Output" });
    }

    TEST(RenderGraphTests, RetainsExplicitSideEffectsAndPassesWithoutWrites)
    {
        const RGResourceHandle transient{ 0 };
        Vector<RGResourceNode> resources(1);
        resources[0].Handle = transient;
        resources[0].Type = RGResourceType::Buffer;

        Vector<RGPassNode> passes;
        passes.push_back(MakePass("Explicit side effect"));
        passes.back().BufferWrites.push_back(transient);
        passes.back().HasSideEffects = true;
        passes.push_back(MakePass("Undeclared external work"));

        RenderGraphAlgorithms::CullPasses(passes, resources);

        ExpectOrder(passes, { "Explicit side effect", "Undeclared external work" });
    }

    TEST(RenderGraphTests, CalculatesLifetimesOnlyForLiveResourceUses)
    {
        const RGResourceHandle liveResource{ 0 };
        const RGResourceHandle unusedResource{ 1 };
        Vector<RGResourceNode> resources(2);
        resources[0].Handle = liveResource;
        resources[1].Handle = unusedResource;

        Vector<RGPassNode> passes;
        passes.push_back(MakePass("Producer"));
        passes.back().BufferWrites.push_back(liveResource);
        passes.push_back(MakePass("Consumer"));
        passes.back().Reads.push_back(liveResource);

        RenderGraphAlgorithms::CalculateResourceLifetimes(passes, resources);

        EXPECT_EQ(resources[0].FirstPassIndex, 0);
        EXPECT_EQ(resources[0].LastPassIndex, 1);
        EXPECT_EQ(resources[1].FirstPassIndex, -1);
        EXPECT_EQ(resources[1].LastPassIndex, -1);
    }

    TEST(RenderGraphTests, DumpsEscapedDeterministicDiagnosticsForMultipleGraphs)
    {
        MockGraphicsDevice device;
        RenderGraph firstGraph(device);
        RenderGraph secondGraph(device);

        RHI::BufferDesc bufferDesc;
        bufferDesc.Size = 64;
        firstGraph.CreateResource("Buffer \"one\"\\path", bufferDesc);
        firstGraph.AddPass<int>("Pass \"one\"\nline",
            [](RenderGraphBuilder& builder, int&) { builder.SetSideEffect(); },
            [](const RenderGraphRegistry&, const int&, RHI::ICommandList*) {});
        secondGraph.CreateResource("Second", bufferDesc);

        const auto directory = std::filesystem::temp_directory_path() /
            ("mixture-render-graph-" + std::to_string(reinterpret_cast<uintptr_t>(&device)));
        const auto firstPath = directory / "first.json";
        const auto secondPath = directory / "second.json";

        ASSERT_TRUE(firstGraph.DumpDiagnostics(firstPath));
        ASSERT_TRUE(secondGraph.DumpDiagnostics(secondPath));

        auto readFile = [](const std::filesystem::path& path)
        {
            std::ifstream input(path);
            return std::string(std::istreambuf_iterator<char>(input), {});
        };

        const std::string firstDump = readFile(firstPath);
        EXPECT_NE(firstDump.find("Buffer \\\"one\\\"\\\\path"), std::string::npos);
        EXPECT_NE(firstDump.find("Pass \\\"one\\\"\\nline"), std::string::npos);
        EXPECT_NE(readFile(secondPath).find("\"name\": \"Second\""), std::string::npos);

        ASSERT_TRUE(firstGraph.DumpDiagnostics(firstPath));
        EXPECT_EQ(readFile(firstPath), firstDump);

        std::error_code error;
        std::filesystem::remove_all(directory, error);
    }

    TEST(RenderGraphResourceCacheTests, ReusesDescriptorsWithoutDependingOnLogicalNames)
    {
        MockGraphicsDevice device;
        RenderGraphResourceCache cache(device);
        RHI::TextureDesc firstDesc;
        firstDesc.Width = 128;
        firstDesc.Height = 128;
        firstDesc.DebugName = "First logical name";

        cache.BeginFrame(0);
        auto first = cache.GetOrCreateTexture(firstDesc);

        RHI::TextureDesc renamedDesc = firstDesc;
        renamedDesc.DebugName = "Different logical name";
        cache.BeginFrame(0);
        auto reused = cache.GetOrCreateTexture(renamedDesc);

        EXPECT_EQ(first.get(), reused.get());
        EXPECT_EQ(device.TextureCreationCount, 1);
    }

    TEST(RenderGraphResourceCacheTests, KeepsConcurrentVirtualResourcesDistinct)
    {
        MockGraphicsDevice device;
        RenderGraphResourceCache cache(device);
        RHI::BufferDesc desc;
        desc.Size = 1024;
        desc.Usage = RHI::BufferUsage::Storage;

        cache.BeginFrame(0);
        auto first = cache.GetOrCreateBuffer(desc);
        auto second = cache.GetOrCreateBuffer(desc);

        EXPECT_NE(first.get(), second.get());
        EXPECT_EQ(device.BufferCreationCount, 2);
    }

    TEST(RenderGraphResourceCacheTests, RetiresResourcesUnusedForACompletedFrameSlot)
    {
        MockGraphicsDevice device;
        RenderGraphResourceCache cache(device);
        RHI::TextureDesc desc;

        cache.BeginFrame(0);
        std::weak_ptr<RHI::ITexture> retired = cache.GetOrCreateTexture(desc);
        ASSERT_FALSE(retired.expired());

        cache.BeginFrame(0); // Mark the entry unused for this completed slot.
        EXPECT_FALSE(retired.expired());
        cache.BeginFrame(0); // The slot is synchronized again; retirement is safe.
        EXPECT_TRUE(retired.expired());
    }

    TEST(RenderGraphRegistryTests, ReleasesTransientMappingsAtTheEndOfTheirLifetime)
    {
        RenderGraphRegistry registry;
        RHI::TextureDesc textureDesc;
        RHI::BufferDesc bufferDesc;
        MockTexture texture(textureDesc);
        MockBuffer buffer(bufferDesc);
        const RGResourceHandle textureHandle{ 0 };
        const RGResourceHandle bufferHandle{ 1 };

        registry.RegisterTexture(textureHandle, &texture);
        registry.RegisterBuffer(bufferHandle, &buffer);
        ASSERT_EQ(registry.GetTexture(textureHandle), &texture);
        ASSERT_EQ(registry.GetBuffer(bufferHandle), &buffer);

        registry.UnregisterTexture(textureHandle);
        registry.UnregisterBuffer(bufferHandle);
        EXPECT_EQ(registry.GetTexture(textureHandle), nullptr);
        EXPECT_EQ(registry.GetBuffer(bufferHandle), nullptr);
    }

    TEST(RenderServiceLifecycleTests, InitializationAndShutdownAreIdempotent)
    {
        MockGraphicsDevice device;
        AssetManager::Get().Init();

        PipelineCache::Init(device);
        PipelineCache::Init(device);
        ShaderLibrary::Init(device);
        ShaderLibrary::Init(device);

        EXPECT_TRUE(PipelineCache::IsInitialized());
        EXPECT_TRUE(ShaderLibrary::IsInitialized());

        ShaderLibrary::Shutdown();
        ShaderLibrary::Shutdown();
        PipelineCache::Shutdown();
        PipelineCache::Shutdown();

        EXPECT_FALSE(PipelineCache::IsInitialized());
        EXPECT_FALSE(ShaderLibrary::IsInitialized());
        AssetManager::Get().Shutdown();
    }

    TEST(VulkanResourceLifetimeTests, ResourceConstructorsRequireSharedDeviceOwnership)
    {
        using namespace Vulkan;

        static_assert(std::is_base_of_v<std::enable_shared_from_this<Device>, Device>);
        static_assert(std::is_constructible_v<Device, Ref<Instance>, Ref<PhysicalDevice>>);
        static_assert(!std::is_constructible_v<Device, Instance&, PhysicalDevice&>);

        static_assert(std::is_constructible_v<Buffer, Ref<Device>, const RHI::BufferDesc&, const void*>);
        static_assert(!std::is_constructible_v<Buffer, const RHI::BufferDesc&, const void*>);

        static_assert(std::is_constructible_v<Texture, Ref<Device>, const RHI::TextureDesc&, const void*>);
        static_assert(!std::is_constructible_v<Texture, const RHI::TextureDesc&, const void*>);

        static_assert(std::is_constructible_v<Shader, Ref<Device>, const void*, size_t,
            RHI::ShaderStage, RHI::ShaderIdentity>);
        static_assert(!std::is_constructible_v<Shader, const void*, size_t, RHI::ShaderStage,
            RHI::ShaderIdentity>);

        static_assert(std::is_constructible_v<Pipeline, Ref<Device>, const RHI::PipelineDesc&>);
        static_assert(!std::is_constructible_v<Pipeline, const RHI::PipelineDesc&>);

        SUCCEED();
    }

    TEST(VulkanAllocationPolicyTests, OrdinaryResourcesRemainEligibleForSuballocation)
    {
        const VmaAllocationCreateInfo allocation = Vulkan::AllocationPolicy::DeviceLocal();

        EXPECT_EQ(allocation.usage, VMA_MEMORY_USAGE_AUTO);
        EXPECT_EQ(allocation.flags & VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, 0u);
    }

    TEST(VulkanAllocationPolicyTests, UploadBuffersRemainSuballocatable)
    {
        const VmaAllocationCreateInfo transient = Vulkan::AllocationPolicy::Upload();
        const VmaAllocationCreateInfo mapped = Vulkan::AllocationPolicy::Upload(true);

        EXPECT_NE(transient.flags & VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, 0u);
        EXPECT_EQ(transient.flags & VMA_ALLOCATION_CREATE_MAPPED_BIT, 0u);
        EXPECT_EQ(transient.flags & VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, 0u);

        EXPECT_NE(mapped.flags & VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, 0u);
        EXPECT_NE(mapped.flags & VMA_ALLOCATION_CREATE_MAPPED_BIT, 0u);
        EXPECT_EQ(mapped.flags & VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, 0u);
    }

    TEST(VulkanUploadTests, UploadAPIIsAsynchronousAndInstrumented)
    {
        using SubmitResult = decltype(Vulkan::SingleTimeCommand::Submit(
            std::declval<const std::function<void(vk::CommandBuffer)>&>()));
        static_assert(std::is_same_v<SubmitResult, Vulkan::SingleTimeCommand::Completion>);
        static_assert(std::is_same_v<Vulkan::SingleTimeCommand::Completion, std::shared_future<void>>);

        Vulkan::SingleTimeCommand::Statistics statistics;
        EXPECT_EQ(statistics.UploadCount, 0u);
        EXPECT_EQ(statistics.BatchCount, 0u);
        EXPECT_EQ(statistics.CommandBufferCount, 0u);
        EXPECT_EQ(statistics.FenceCount, 0u);
    }

    TEST(GraphicsContextContractTests, SupportsAContextWithoutImGui)
    {
        static_assert(!HasImGuiLifecycle<RHI::IGraphicsContext>);
        static_assert(std::is_default_constructible_v<HeadlessGraphicsContext>);

        HeadlessGraphicsContext context;
        EXPECT_EQ(context.GetAPI(), RHI::GraphicsAPI::None);
        EXPECT_EQ(context.BeginFrame(), nullptr);
        EXPECT_EQ(context.GetCommandBuffer(), nullptr);
    }

    TEST(PipelineCacheTests, ShaderReloadReleasesAndRecreatesDependentPipelines)
    {
        MockGraphicsDevice device;
        PipelineCache::Init(device);
        ShaderLibrary::Init(device);

        const UUID shaderAssetID(0xC0FFEE);
        MockShader firstVersion({ static_cast<uint64_t>(shaderAssetID), 1, RHI::ShaderStage::Vertex });
        RHI::PipelineDesc desc;
        desc.VertexShader = &firstVersion;

        RHI::IPipeline* firstPipeline = PipelineCache::GetPipeline(desc);
        ASSERT_NE(firstPipeline, nullptr);
        EXPECT_EQ(device.PipelineCreationCount, 1u);
        EXPECT_EQ(device.PipelineDestructionCount, 0u);

        MockShader equivalentFirstVersion(
            { static_cast<uint64_t>(shaderAssetID), 1, RHI::ShaderStage::Vertex });
        desc.VertexShader = &equivalentFirstVersion;
        EXPECT_EQ(PipelineCache::GetPipeline(desc), firstPipeline);
        EXPECT_EQ(device.PipelineCreationCount, 1u);

        ShaderLibrary::Reload(AssetHandle{ shaderAssetID, 0 });
        EXPECT_EQ(device.PipelineDestructionCount, 1u);

        MockShader secondVersion({ static_cast<uint64_t>(shaderAssetID), 2, RHI::ShaderStage::Vertex });
        desc.VertexShader = &secondVersion;
        ASSERT_NE(PipelineCache::GetPipeline(desc), nullptr);
        EXPECT_EQ(device.PipelineCreationCount, 2u);

        ShaderLibrary::Shutdown();
        PipelineCache::Shutdown();
        EXPECT_EQ(device.PipelineDestructionCount, 2u);
    }

}
