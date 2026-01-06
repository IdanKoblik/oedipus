#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include <memory>
#include "editor.h"
#include "config/config.h"

using namespace editor;
using namespace config;

class EditorTest : public ::testing::Test {
protected:
    void SetUp() override {
        char tmpl[] = "/tmp/cake_test_XXXXXX";
        const int fd = mkstemp(tmpl);
        ASSERT_NE(fd, -1);

        close(fd);
        test_file = tmpl;
        cfg = Config();
        
        std::ofstream file(test_file);
        file << "Line 1\nLine 2\nLine 3";
        file.close();
    }

    void TearDown() override {
        if (std::filesystem::exists(test_file)) {
            std::filesystem::remove(test_file);
        }
    }

    std::unique_ptr<TextEditor> createEditor() {
        try {
            return std::make_unique<TextEditor>(cfg);
        } catch (const std::runtime_error&) {
            return nullptr;
        }
    }

    std::string test_file;
    Config cfg;
};

TEST_F(EditorTest, Constructor) {
    auto editor = createEditor();
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    EXPECT_EQ(editor->state.mode, PHILOSOPHICAL);
    EXPECT_EQ(editor->state.cursor.x, 1);
    EXPECT_EQ(editor->state.cursor.y, 0);
}

TEST_F(EditorTest, OpenFile) {
    auto editor = createEditor();
    if (!editor) {
        GTEST_SKIP() << "Not running in a terminal environment";
        return;
    }
    editor->openFile(test_file);
    
    EXPECT_EQ(editor->path, test_file);
    EXPECT_EQ(editor->cake.lineCount(), 3);
    EXPECT_EQ(editor->cake.renderLine(0), "Line 1");
}

TEST_F(EditorTest, PushUndo) {
    auto editor = createEditor();
    if (!editor) return;
    editor->openFile(test_file);
    editor->state.cursor = Cursor{2, 0};
    
    editor->pushUndo();
    
    EXPECT_EQ(editor->undoStack.size(), 1);
    EXPECT_TRUE(editor->redoStack.empty());
}

TEST_F(EditorTest, Undo_EmptyStack) {
    auto editor = createEditor();
    if (!editor) return;
    editor->openFile(test_file);
    
    editor->undo();
    
    EXPECT_TRUE(editor->undoStack.empty());
}

TEST_F(EditorTest, Undo_SingleAction) {
    auto editor = createEditor();
    if (!editor) return;
    editor->openFile(test_file);
    editor->state.cursor = Cursor{2, 0};
    
    std::string original = editor->cake.renderLine(0);
    editor->pushUndo();
    editor->cake.insertChar(0, 0, 'X');
    
    editor->undo();
    
    EXPECT_EQ(editor->cake.renderLine(0), original);
    EXPECT_EQ(editor->redoStack.size(), 1);
}

TEST_F(EditorTest, Undo_MultipleActions) {
    auto editor = createEditor();
    if (!editor) return;
    editor->openFile(test_file);
    editor->state.cursor = Cursor{1, 0};
    
    std::string original = editor->cake.renderLine(0);
    editor->pushUndo();
    editor->cake.insertChar(0, 0, 'A');
    editor->pushUndo();
    editor->cake.insertChar(1, 0, 'B');
    
    editor->undo();
    EXPECT_EQ(editor->cake.renderLine(0), "ALine 1");
    
    editor->undo();
    EXPECT_EQ(editor->cake.renderLine(0), original);
}

TEST_F(EditorTest, Redo_EmptyStack) {
    auto editor = createEditor();
    if (!editor) return;
    editor->openFile(test_file);
    
    editor->redo(); // Should do nothing
    
    EXPECT_TRUE(editor->redoStack.empty());
}

TEST_F(EditorTest, Redo_SingleAction) {
    auto editor = createEditor();
    if (!editor) return;
    editor->openFile(test_file);
    editor->state.cursor = Cursor{1, 0};
    
    editor->pushUndo();
    editor->cake.insertChar(0, 0, 'X');
    std::string modified = editor->cake.renderLine(0);
    
    editor->undo();
    editor->redo();
    
    EXPECT_EQ(editor->cake.renderLine(0), modified);
    EXPECT_EQ(editor->undoStack.size(), 1);
}

TEST_F(EditorTest, Redo_MultipleActions) {
    auto editor = createEditor();
    if (!editor) return;
    editor->openFile(test_file);
    editor->state.cursor = Cursor{1, 0};
    
    editor->pushUndo();
    editor->cake.insertChar(0, 0, 'A');
    editor->pushUndo();
    editor->cake.insertChar(1, 0, 'B');
    
    std::string final = editor->cake.renderLine(0);
    
    editor->undo();
    editor->undo();
    
    editor->redo();
    EXPECT_EQ(editor->cake.renderLine(0), "ALine 1");
    
    editor->redo();
    EXPECT_EQ(editor->cake.renderLine(0), final);
}

TEST_F(EditorTest, UndoRedo_CursorPosition) {
    auto editor = createEditor();
    if (!editor) return;
    editor->openFile(test_file);
    editor->state.cursor = Cursor{3, 1};
    
    Cursor original = editor->state.cursor;
    editor->pushUndo();
    editor->state.cursor = Cursor{5, 2};
    
    editor->undo();
    EXPECT_EQ(editor->state.cursor.x, original.x);
    EXPECT_EQ(editor->state.cursor.y, original.y);
}

TEST_F(EditorTest, PushUndo_ClearsRedoStack) {
    auto editor = createEditor();
    if (!editor) return;
    editor->openFile(test_file);
    editor->state.cursor = Cursor{1, 0};
    
    editor->pushUndo();
    editor->cake.insertChar(0, 0, 'A');
    editor->undo();
    
    EXPECT_EQ(editor->redoStack.size(), 1);
    
    editor->pushUndo();
    EXPECT_TRUE(editor->redoStack.empty());
}

TEST_F(EditorTest, StateInitialization) {
    auto editor = createEditor();
    if (!editor) return;
    
    EXPECT_EQ(editor->state.mode, PHILOSOPHICAL);
    EXPECT_EQ(editor->state.cursor.x, 1);
    EXPECT_EQ(editor->state.cursor.y, 0);
    EXPECT_FALSE(editor->state.search.active);
}

TEST_F(EditorTest, Destructor) {
    {
        auto editor = createEditor();
        if (!editor) return;
        editor->openFile(test_file);
    }
    
    EXPECT_TRUE(std::filesystem::exists(test_file));
}

