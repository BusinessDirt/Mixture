#include <gtest/gtest.h>
#include "Mixture/Events/Event.hpp"
#include "Mixture/Events/ApplicationEvent.hpp"
#include "Mixture/Events/KeyEvent.hpp"
#include "Mixture/Events/MouseEvent.hpp"

namespace Mixture::Tests {

    // --- Event Dispatcher Tests ---

    TEST(EventTests, DispatcherSuccess) {
        WindowResizeEvent e(800, 600);
        EventDispatcher dispatcher(e);
        
        bool dispatched = dispatcher.Dispatch<WindowResizeEvent>([](WindowResizeEvent& ev) {
            EXPECT_EQ(ev.GetWidth(), 800);
            EXPECT_EQ(ev.GetHeight(), 600);
            return true;
        });

        EXPECT_TRUE(dispatched);
        EXPECT_TRUE(e.Handled);
    }

    TEST(EventTests, DispatcherFailTypeMismatch) {
        WindowResizeEvent e(800, 600);
        EventDispatcher dispatcher(e);
        
        bool dispatched = dispatcher.Dispatch<KeyPressedEvent>([](KeyPressedEvent& ev) {
            return true;
        });

        EXPECT_FALSE(dispatched);
        EXPECT_FALSE(e.Handled);
    }

    // --- Application Events ---

    TEST(EventTests, WindowResize) {
        WindowResizeEvent e(1280, 720);
        EXPECT_EQ(e.GetEventType(), EventType::WindowResize);
        EXPECT_EQ(e.GetCategoryFlags(), EventCategoryApplication);
        EXPECT_EQ(e.GetWidth(), 1280);
        EXPECT_EQ(e.GetHeight(), 720);
        EXPECT_STREQ(e.GetName(), "WindowResize");
        EXPECT_TRUE(e.IsInCategory(EventCategoryApplication));
    }

    TEST(EventTests, WindowClose) {
        WindowCloseEvent e;
        EXPECT_EQ(e.GetEventType(), EventType::WindowClose);
        EXPECT_TRUE(e.IsInCategory(EventCategoryApplication));
    }

    // --- Key Events ---

    TEST(EventTests, KeyPressed) {
        KeyPressedEvent e(Key::Space, true);
        EXPECT_EQ(e.GetEventType(), EventType::KeyPressed);
        EXPECT_TRUE(e.IsInCategory(EventCategoryKeyboard));
        EXPECT_TRUE(e.IsInCategory(EventCategoryInput));
        EXPECT_EQ(e.GetKeyCode(), Key::Space);
        EXPECT_TRUE(e.IsRepeat());
        EXPECT_STREQ(e.GetName(), "KeyPressed");
    }

    TEST(EventTests, KeyReleased) {
        KeyReleasedEvent e(Key::A);
        EXPECT_EQ(e.GetEventType(), EventType::KeyReleased);
        EXPECT_EQ(e.GetKeyCode(), Key::A);
        EXPECT_STREQ(e.GetName(), "KeyReleased");
    }

    // --- Mouse Events ---

    TEST(EventTests, MouseMoved) {
        MouseMovedEvent e(100.0f, 200.0f);
        EXPECT_EQ(e.GetEventType(), EventType::MouseMoved);
        EXPECT_TRUE(e.IsInCategory(EventCategoryMouse));
        EXPECT_TRUE(e.IsInCategory(EventCategoryInput));
        EXPECT_FLOAT_EQ(e.GetX(), 100.0f);
        EXPECT_FLOAT_EQ(e.GetY(), 200.0f);
    }

    TEST(EventTests, MouseScrolled) {
        MouseScrolledEvent e(0.5f, -1.0f);
        EXPECT_EQ(e.GetEventType(), EventType::MouseScrolled);
        EXPECT_FLOAT_EQ(e.GetXOffset(), 0.5f);
        EXPECT_FLOAT_EQ(e.GetYOffset(), -1.0f);
    }

    TEST(EventTests, MouseButton) {
        MouseButtonPressedEvent e(Mouse::ButtonLeft);
        EXPECT_EQ(e.GetEventType(), EventType::MouseButtonPressed);
        EXPECT_TRUE(e.IsInCategory(EventCategoryMouseButton));
        EXPECT_EQ(e.GetMouseButton(), Mouse::ButtonLeft);
    }
}
