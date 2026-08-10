#include <gtest/gtest.h>
#include "Mixture/Core/Base.hpp"
#include "Mixture/Core/Time.hpp"
#include "Mixture/Core/LayerStack.hpp"
#include "Mixture/Core/Application.hpp"
#include "Opal/Log.hpp"
#include <thread>

namespace Mixture::Tests {

    TEST(CoreTests, LoggerLookupUsesThreadLocalFastPath) {
        auto& registry = Opal::LogRegistry::Get();
        const auto before = registry.GetRegistryLookupCount();
        auto logger = registry.GetLogger("Tests/LoggingFastPath");
        const auto afterFirst = registry.GetRegistryLookupCount();
        for (size_t index = 0; index < 10000; ++index)
            EXPECT_EQ(registry.GetLogger("Tests/LoggingFastPath"), logger);

        EXPECT_EQ(afterFirst, before + 1);
        EXPECT_EQ(registry.GetRegistryLookupCount(), afterFirst);
    }

    TEST(CoreTests, LoggerLookupScalesAcrossThreads) {
        auto& registry = Opal::LogRegistry::Get();
        constexpr size_t ThreadCount = 8;
        constexpr size_t LookupsPerThread = 5000;
        const auto before = registry.GetRegistryLookupCount();

        std::vector<std::thread> threads;
        threads.reserve(ThreadCount);
        for (size_t threadIndex = 0; threadIndex < ThreadCount; ++threadIndex) {
            threads.emplace_back([&registry] {
                for (size_t lookup = 0; lookup < LookupsPerThread; ++lookup)
                    registry.GetLogger("Tests/MultithreadedLogging");
            });
        }
        for (auto& thread : threads) thread.join();

        EXPECT_LE(registry.GetRegistryLookupCount() - before, ThreadCount);
    }

    // --- Base.hpp Tests ---

    TEST(CoreTests, ScopeCreation) {
        auto scope = CreateScope<int>(42);
        EXPECT_NE(scope, nullptr);
        EXPECT_EQ(*scope, 42);
    }

    TEST(CoreTests, RefCreation) {
        auto ref = CreateRef<int>(100);
        EXPECT_NE(ref, nullptr);
        EXPECT_EQ(*ref, 100);
        EXPECT_EQ(ref.use_count(), 1);
        auto ref2 = ref;
        EXPECT_EQ(ref.use_count(), 2);
    }

    // --- Time.hpp Tests ---

    TEST(CoreTests, TimerFunctionality) {
        Timer timer;
        EXPECT_GE(timer.Elapsed(), 0.0f);
        
        // Sleep for a short duration to ensure timer advances
        // Note: Sleep might not be precise, so we check for > 0
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        EXPECT_GT(timer.Elapsed(), 0.001f);
        EXPECT_GT(timer.ElapsedMillis(), 1.0f);
        
        float delta = timer.Tick();
        EXPECT_GT(delta, 0.001f);
        
        // After tick, elapsed should be near 0 relative to previous Tick, 
        // but it keeps counting.
        EXPECT_GE(timer.Elapsed(), 0.0f);
    }

    // --- LayerStack.hpp Tests ---

    class MockLayer : public Layer {
    public:
        MockLayer(const std::string& name) : Layer(name) {}
        void OnAttach() override { AttachCount++; }
        void OnDetach() override { DetachCount++; }
        void OnEvent(Event& event) override {}

        int AttachCount = 0;
        int DetachCount = 0;
    };

    TEST(CoreTests, LayerStackPushPop) {
        LayerStack stack;
        auto layer1Owner = CreateScope<MockLayer>("Layer1");
        auto layer2Owner = CreateScope<MockLayer>("Layer2");
        auto overlay1Owner = CreateScope<MockLayer>("Overlay1");
        auto* layer1 = layer1Owner.get();
        auto* layer2 = layer2Owner.get();
        auto* overlay1 = overlay1Owner.get();

        stack.PushLayer(std::move(layer1Owner));
        stack.PushLayer(std::move(layer2Owner));
        stack.PushOverlay(std::move(overlay1Owner));

        // Order should be: Layer1, Layer2, Overlay1
        // Vector: [Layer1, Layer2, Overlay1]
        // LayerInsertIndex should point to Overlay1 (index 2)
        
        auto it = stack.begin();
        EXPECT_EQ(it->get(), layer1);
        EXPECT_EQ((++it)->get(), layer2);
        EXPECT_EQ((++it)->get(), overlay1);

        auto removedLayer = stack.PopLayer(layer1);
        // Expect: Layer2, Overlay1
        it = stack.begin();
        EXPECT_EQ(it->get(), layer2);
        EXPECT_EQ((++it)->get(), overlay1);

        auto removedOverlay = stack.PopOverlay(overlay1);
        // Expect: Layer2
        it = stack.begin();
        EXPECT_EQ(it->get(), layer2);
        EXPECT_EQ(stack.end(), ++it);

        EXPECT_EQ(removedLayer.get(), layer1);
        EXPECT_EQ(removedOverlay.get(), overlay1);
        stack.Shutdown();
        stack.Shutdown();
    }

    TEST(CoreTests, LayerStackIgnoresInvalidRemoval) {
        LayerStack stack;
        auto layerOwner = CreateScope<MockLayer>("Layer");
        auto overlayOwner = CreateScope<MockLayer>("Overlay");
        auto absent = CreateScope<MockLayer>("Absent");
        auto* layer = layerOwner.get();
        auto* overlay = overlayOwner.get();

        stack.PushLayer(std::move(layerOwner));
        stack.PushOverlay(std::move(overlayOwner));

        EXPECT_EQ(stack.PopLayer(absent.get()), nullptr);
        EXPECT_EQ(stack.PopOverlay(absent.get()), nullptr);
        EXPECT_EQ(stack.PopLayer(overlay), nullptr);
        EXPECT_EQ(stack.PopOverlay(layer), nullptr);

        EXPECT_EQ(layer->DetachCount, 0);
        EXPECT_EQ(overlay->DetachCount, 0);
        EXPECT_EQ(std::distance(stack.begin(), stack.end()), 2);

        auto removedLayer = stack.PopLayer(layer);
        auto removedOverlay = stack.PopOverlay(overlay);
        EXPECT_EQ(layer->DetachCount, 1);
        EXPECT_EQ(overlay->DetachCount, 1);
        EXPECT_NE(removedLayer, nullptr);
        EXPECT_NE(removedOverlay, nullptr);
        EXPECT_THROW(stack.PushLayer(nullptr), std::invalid_argument);
        EXPECT_THROW(stack.PushOverlay(nullptr), std::invalid_argument);
    }

    // --- Application.hpp Tests ---
    
    TEST(CoreTests, CommandLineArgs) {
        const char* argv[] = { "app_name", "--arg1", "value1" };
        ApplicationCommandLineArgs args;
        args.Count = 3;
        args.Args = const_cast<char**>(argv);

        EXPECT_STREQ(args[0], "app_name");
        EXPECT_STREQ(args[1], "--arg1");
        EXPECT_STREQ(args[2], "value1");
    }

    TEST(CoreTests, CommandLineArgsRejectInvalidIndicesInReleaseBuilds)
    {
        char arg0[] = "app";
        char* values[] = { arg0 };
        ApplicationCommandLineArgs args{ 1, values };
        EXPECT_THROW(args[-1], std::out_of_range);
        EXPECT_THROW(args[1], std::out_of_range);
        EXPECT_THROW((ApplicationCommandLineArgs{ 1, nullptr }[0]), std::out_of_range);
    }

    TEST(CoreTests, WindowRejectsInvalidDimensionsBeforePlatformStartup)
    {
        EXPECT_THROW(Window(WindowProps("Invalid", 0, 480)), std::invalid_argument);
    }

}
