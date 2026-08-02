#include <gtest/gtest.h>

#include "Mixture/Render/Graph/RenderGraphDefinitions.hpp"
#include "Mixture/Render/Graph/RenderGraphResourceCache.hpp"
#include "Mixture/Render/Graph/RenderGraphRegistry.hpp"

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

        class MockGraphicsDevice final : public RHI::IGraphicsDevice
        {
        public:
            Ref<RHI::IShader> CreateShader(const void*, size_t, RHI::ShaderStage) override
            {
                return nullptr;
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
                return CreateRef<RHI::IPipeline>();
            }

            void WaitForIdle() override {}

            size_t BufferCreationCount = 0;
            size_t TextureCreationCount = 0;
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
}
