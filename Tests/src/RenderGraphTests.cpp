#include <gtest/gtest.h>

#include "Mixture/Render/Graph/RenderGraphDefinitions.hpp"

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
}
