#include <gtest/gtest.h>
#include "Mixture/Core/Base.hpp"
#include "Mixture/Core/Time.hpp"
#include "Mixture/Core/LayerStack.hpp"
#include "Mixture/Core/Application.hpp"
#include <thread>

namespace Mixture::Tests {

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
        void OnAttach() override {}
        void OnDetach() override {}
        void OnEvent(Event& event) override {}
    };

    TEST(CoreTests, LayerStackPushPop) {
        LayerStack stack;
        auto* layer1 = new MockLayer("Layer1");
        auto* layer2 = new MockLayer("Layer2");
        auto* overlay1 = new MockLayer("Overlay1");

        stack.PushLayer(layer1);
        stack.PushLayer(layer2);
        stack.PushOverlay(overlay1);

        // Order should be: Layer1, Layer2, Overlay1
        // Vector: [Layer1, Layer2, Overlay1]
        // LayerInsertIndex should point to Overlay1 (index 2)
        
        auto it = stack.begin();
        EXPECT_EQ(*it, layer1);
        EXPECT_EQ(*(++it), layer2);
        EXPECT_EQ(*(++it), overlay1);

        stack.PopLayer(layer1);
        // Expect: Layer2, Overlay1
        it = stack.begin();
        EXPECT_EQ(*it, layer2);
        EXPECT_EQ(*(++it), overlay1);

        stack.PopOverlay(overlay1);
        // Expect: Layer2
        it = stack.begin();
        EXPECT_EQ(*it, layer2);
        EXPECT_EQ(stack.end(), ++it);
        
        // Cleanup (LayerStack doesn't own memory in this implementation usually, 
        // but typically Application owns it. In this test, we must delete if LayerStack doesn't delete on Pop.
        // Looking at LayerStack.cpp (not provided but implied), Pop usually just removes from vector.
        // User is responsible for deletion if they allocated with new.
        // Wait, LayerStack::Shutdown usually deletes. But we popped them.
        delete layer1;
        delete layer2;
        delete overlay1;
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

}
