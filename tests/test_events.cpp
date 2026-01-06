#include <gtest/gtest.h>
#include "events/event.h"
#include "events/keyboard_event.h"
#include "events/movement_event.h"
#include "events/mode_event.h"
#include "events/search_event.h"
#include "listeners/listener.h"

using namespace event;
using namespace listener;

class MockKeyboardListener : public IListener<KeyboardEvent> {
public:
    bool called = false;
    char received_key = 0;

    void handle(const KeyboardEvent& e) override {
        called = true;
        received_key = e.key;
    }
};

class MockMovementListener : public IListener<MovementEvent> {
public:
    bool called = false;
    char received_key = 0;

    void handle(const MovementEvent& e) override {
        called = true;
        received_key = e.key;
    }
};

class MockModeListener : public IListener<ModeEvent> {
public:
    bool called = false;
    editor::EditorMode received_mode = editor::WRITING;

    void handle(const ModeEvent& e) override {
        called = true;
        received_mode = e.mode;
    }
};

class MockSearchListener : public IListener<SearchEvent> {
public:
    bool called = false;
    std::string received_target;

    void handle(const SearchEvent& e) override {
        called = true;
        received_target = e.target;
    }
};

class EventDispatcherTest : public ::testing::Test {
protected:
    void SetUp() override {
        EventDispatcher::instance().clearListeners();
    }
};


TEST_F(EventDispatcherTest, SingletonInstance) {
    EventDispatcher& instance1 = EventDispatcher::instance();
    EventDispatcher& instance2 = EventDispatcher::instance();
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(EventDispatcherTest, RegisterListener) {
    EventDispatcher& dispatcher = EventDispatcher::instance();
    MockKeyboardListener listener;
    
    dispatcher.registerListener(&listener);
    
    KeyboardEvent event('a');
    dispatcher.fire(event);
    
    EXPECT_TRUE(listener.called);
    EXPECT_EQ(listener.received_key, 'a');
}

TEST_F(EventDispatcherTest, FireKeyboardEvent) {
    EventDispatcher& dispatcher = EventDispatcher::instance();
    MockKeyboardListener listener;
    
    dispatcher.registerListener(&listener);
    
    KeyboardEvent event('x');
    dispatcher.fire(event);
    
    EXPECT_TRUE(listener.called);
    EXPECT_EQ(listener.received_key, 'x');
}

TEST_F(EventDispatcherTest, FireMovementEvent) {
    EventDispatcher& dispatcher = EventDispatcher::instance();
    MockMovementListener listener;
    
    dispatcher.registerListener(&listener);
    
    MovementEvent event('h');
    dispatcher.fire(event);
    
    EXPECT_TRUE(listener.called);
    EXPECT_EQ(listener.received_key, 'h');
}

TEST_F(EventDispatcherTest, FireModeEvent) {
    EventDispatcher& dispatcher = EventDispatcher::instance();
    MockModeListener listener;
    
    dispatcher.registerListener(&listener);
    
    ModeEvent event(editor::PHILOSOPHICAL);
    dispatcher.fire(event);
    
    EXPECT_TRUE(listener.called);
    EXPECT_EQ(listener.received_mode, editor::PHILOSOPHICAL);
}

TEST_F(EventDispatcherTest, FireSearchEvent) {
    EventDispatcher& dispatcher = EventDispatcher::instance();
    MockSearchListener listener;

    dispatcher.registerListener(&listener);

    SearchEvent event("test");
    dispatcher.fire(event);

    EXPECT_TRUE(listener.called);
    EXPECT_EQ(listener.received_target, "test");
}

TEST_F(EventDispatcherTest, MultipleListeners) {
    EventDispatcher& dispatcher = EventDispatcher::instance();
    MockKeyboardListener listener1;
    MockKeyboardListener listener2;
    
    dispatcher.registerListener(&listener1);
    dispatcher.registerListener(&listener2);
    
    KeyboardEvent event('z');
    dispatcher.fire(event);
    
    EXPECT_TRUE(listener1.called);
    EXPECT_TRUE(listener2.called);
    EXPECT_EQ(listener1.received_key, 'z');
    EXPECT_EQ(listener2.received_key, 'z');
}

TEST_F(EventDispatcherTest, WrongListenerTypeNotCalled) {
    EventDispatcher& dispatcher = EventDispatcher::instance();
    MockKeyboardListener keyboardListener;
    MockMovementListener movementListener;
    
    dispatcher.registerListener(&keyboardListener);
    dispatcher.registerListener(&movementListener);
    
    KeyboardEvent event('a');
    dispatcher.fire(event);
    
    EXPECT_TRUE(keyboardListener.called);
    EXPECT_FALSE(movementListener.called);
}

TEST_F(EventDispatcherTest, MovementEventInheritsFromKeyboardEvent) {
    EventDispatcher& dispatcher = EventDispatcher::instance();
    MockKeyboardListener keyboardListener;
    MockMovementListener movementListener;
    
    dispatcher.registerListener(&keyboardListener);
    dispatcher.registerListener(&movementListener);
    
    MovementEvent event('j');
    dispatcher.fire(event);
    
    EXPECT_FALSE(keyboardListener.called);
    EXPECT_TRUE(movementListener.called);
    EXPECT_EQ(movementListener.received_key, 'j');
}

TEST(KeyboardEventTest, Constructor) {
    KeyboardEvent event('t');
    EXPECT_EQ(event.key, 't');
}

TEST(MovementEventTest, Constructor) {
    MovementEvent event('k');
    EXPECT_EQ(event.key, 'k');
}

TEST(ModeEventTest, Constructor) {
    ModeEvent event(editor::WRITING);
    EXPECT_EQ(event.mode, editor::WRITING);
}

TEST(SearchEventTest, Constructor) {
    SearchEvent event("search term");
    EXPECT_EQ(event.target, "search term");
}

TEST(SearchEventTest, EmptyString) {
    SearchEvent event("");
    EXPECT_EQ(event.target, "");
}
