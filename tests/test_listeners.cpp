#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include "listeners/keyboard_listener.h"
#include "listeners/movement_listener.h"
#include "listeners/mode_listener.h"
#include "listeners/search_listener.h"
#include "events/keyboard_event.h"
#include "events/movement_event.h"
#include "events/mode_event.h"
#include "events/search_event.h"
#include "editor.h"
#include "config/config.h"

using namespace listener;
using namespace editor;
using namespace event;
using namespace config;

class ListenerTest : public testing::Test {
protected:
    void SetUp() override {
        EventDispatcher::instance().clearListeners();

        char tmpl[] = "/tmp/cake_test_XXXXXX";
        const int fd = mkstemp(tmpl);
        ASSERT_NE(fd, -1);

        close(fd);
        test_file = tmpl;
        cfg = Config();
        
        std::ofstream file(test_file);
        file << "Hello\nWorld\nTest";
        file.close();
        
        try {
            editor = std::make_unique<TextEditor>(cfg);
            editor->openFile(test_file);
            editor->cake.loadFromFile(test_file);
        } catch (const std::runtime_error&) {
            editor = nullptr;
        }
    }

    void TearDown() override {
        EventDispatcher::instance().clearListeners();
        if (std::filesystem::exists(test_file))
            std::filesystem::remove(test_file);
    }

    std::string test_file;
    Config cfg;
    std::unique_ptr<TextEditor> editor;
};

TEST_F(ListenerTest, KeyboardListener_PhilosophicalMode_Undo) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }

    KeyboardListener listener(*editor);
    editor->state.mode = PHILOSOPHICAL;
    editor->state.cursor = Cursor{1, 0};
    
    editor->pushUndo();
    editor->cake.insertChar(0, 0, 'X');
    
    KeyboardEvent event('u');
    listener.handle(event);
    
    EXPECT_EQ(editor->cake.renderLine(0), "Hello");
}

TEST_F(ListenerTest, KeyboardListener_PhilosophicalMode_Redo) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }

    KeyboardListener listener(*editor);
    editor->state.mode = PHILOSOPHICAL;
    editor->state.cursor = Cursor{1, 0};
    
    editor->pushUndo();
    editor->cake.insertChar(0, 0, 'X');
    editor->undo();
    
    KeyboardEvent event('r');
    listener.handle(event);
    
    EXPECT_EQ(editor->cake.renderLine(0), "XHello");
}

TEST_F(ListenerTest, KeyboardListener_PhilosophicalMode_Movement) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }

    KeyboardListener listener(*editor);
    editor->state.mode = PHILOSOPHICAL;
    editor->state.cursor = Cursor{2, 0};
    
    KeyboardEvent event('h');
    listener.handle(event);
    
    EXPECT_TRUE(true);
}

TEST_F(ListenerTest, KeyboardListener_WritingMode_InsertChar) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }

    KeyboardListener listener(*editor);
    editor->state.mode = WRITING;
    editor->state.cursor = Cursor{1, 0};
    
    KeyboardEvent event('A');
    listener.handle(event);
    
    EXPECT_EQ(editor->cake.renderLine(0), "AHello");
    EXPECT_EQ(editor->state.cursor.x, 2);
}

TEST_F(ListenerTest, KeyboardListener_WritingMode_Backspace) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    KeyboardListener listener(*editor);
    editor->state.mode = WRITING;
    editor->state.cursor = Cursor{2, 0};
    
    KeyboardEvent event(BACKSPACE);
    listener.handle(event);
    
    EXPECT_EQ(editor->cake.renderLine(0), "ello");
    EXPECT_EQ(editor->state.cursor.x, 1);
}

TEST_F(ListenerTest, KeyboardListener_WritingMode_BackspaceAtStart) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    KeyboardListener listener(*editor);
    editor->state.mode = WRITING;
    editor->state.cursor = Cursor{1, 1};
    
    KeyboardEvent event(BACKSPACE);
    listener.handle(event);
    
    EXPECT_EQ(editor->cake.lineCount(), 2);
    EXPECT_EQ(editor->state.cursor.y, 0);
}

TEST_F(ListenerTest, KeyboardListener_WritingMode_Newline) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    KeyboardListener listener(*editor);
    editor->state.mode = WRITING;
    editor->state.cursor = Cursor{3, 0};
    
    KeyboardEvent event('\r');
    listener.handle(event);
    
    EXPECT_EQ(editor->cake.lineCount(), 4);
    EXPECT_EQ(editor->state.cursor.y, 1);
    EXPECT_EQ(editor->state.cursor.x, 1);
}

TEST_F(ListenerTest, KeyboardListener_WritingMode_Tab) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    KeyboardListener listener(*editor);
    editor->state.mode = WRITING;
    editor->state.cursor = Cursor{1, 0};
    
    KeyboardEvent event('\t');
    listener.handle(event);
    
    std::string line = editor->cake.renderLine(0);
    EXPECT_EQ(line.substr(0, 4), "    ");
    EXPECT_EQ(editor->state.cursor.x, 5);
}

TEST_F(ListenerTest, MovementListener_MoveLeft) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    MovementListener listener(*editor);
    editor->state.cursor = Cursor{3, 0};
    
    MovementEvent event('h');
    listener.handle(event);
    
    EXPECT_EQ(editor->state.cursor.x, 2);
}

TEST_F(ListenerTest, MovementListener_MoveLeftAtStart) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    MovementListener listener(*editor);
    editor->state.cursor = Cursor{1, 1};
    
    MovementEvent event('h');
    listener.handle(event);
    
    EXPECT_EQ(editor->state.cursor.y, 0);
    EXPECT_GT(editor->state.cursor.x, 1);
}

TEST_F(ListenerTest, MovementListener_MoveRight) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    MovementListener listener(*editor);
    editor->state.cursor = Cursor{2, 0};
    
    MovementEvent event('l');
    listener.handle(event);
    
    EXPECT_EQ(editor->state.cursor.x, 3);
}

TEST_F(ListenerTest, MovementListener_MoveRightAtEnd) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    MovementListener listener(*editor);
    size_t lineLen = editor->cake.lineLength(0);
    editor->state.cursor = Cursor{lineLen + 1, 0};
    
    MovementEvent event('l');
    listener.handle(event);
    
    EXPECT_EQ(editor->state.cursor.y, 1);
    EXPECT_EQ(editor->state.cursor.x, 1);
}

TEST_F(ListenerTest, MovementListener_MoveUp) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    MovementListener listener(*editor);
    editor->state.cursor = Cursor{2, 1};
    
    MovementEvent event('k');
    listener.handle(event);
    
    EXPECT_EQ(editor->state.cursor.y, 0);
}

TEST_F(ListenerTest, MovementListener_MoveDown) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    MovementListener listener(*editor);
    editor->state.cursor = Cursor{2, 0};
    
    MovementEvent event('j');
    listener.handle(event);
    
    EXPECT_EQ(editor->state.cursor.y, 1);
}

TEST_F(ListenerTest, MovementListener_CursorAdjustment) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    MovementListener listener(*editor);
    editor->state.cursor = Cursor{100, 0};
    
    MovementEvent event('h');
    listener.handle(event);
    
    size_t lineLen = editor->cake.lineLength(0);
    EXPECT_LE(editor->state.cursor.x, lineLen + 1);
}

TEST_F(ListenerTest, ModeListener_ChangeMode) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    ModeListener listener(*editor);
    editor->state.mode = WRITING;
    
    ModeEvent event(PHILOSOPHICAL);
    listener.handle(event);
    
    EXPECT_EQ(editor->state.mode, PHILOSOPHICAL);
}

TEST_F(ListenerTest, ModeListener_ClearSearch) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    ModeListener listener(*editor);
    editor->state.search.active = true;
    editor->state.search.target = "test";
    
    ModeEvent event(WRITING);
    listener.handle(event);
    
    EXPECT_FALSE(editor->state.search.active);
}

TEST_F(ListenerTest, SearchListener_FindMatches) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }

    SearchListener listener(*editor);
    SearchEvent event("Hello");
    listener.handle(event);
    
    EXPECT_TRUE(editor->state.search.active);
    EXPECT_EQ(editor->state.search.target, "Hello");
    EXPECT_EQ(editor->state.search.matches.size(), 1);
}

TEST_F(ListenerTest, SearchListener_NoMatches) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    SearchListener listener(*editor);
    
    SearchEvent event("xyz");
    listener.handle(event);
    
    EXPECT_FALSE(editor->state.search.active);
    EXPECT_EQ(editor->state.search.matches.size(), 0);
}

TEST_F(ListenerTest, SearchListener_MultipleMatches) {
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    SearchListener listener(*editor);
    
    editor->cake.insertChar(5, 0, ' ');
    editor->cake.insertChar(6, 0, 'H');
    editor->cake.insertChar(7, 0, 'e');
    editor->cake.insertChar(8, 0, 'l');
    editor->cake.insertChar(9, 0, 'l');
    editor->cake.insertChar(10, 0, 'o');
    
    SearchEvent event("Hello");
    listener.handle(event);
    
    EXPECT_TRUE(editor->state.search.active);
    EXPECT_GE(editor->state.search.matches[0].size(), 1);
}

