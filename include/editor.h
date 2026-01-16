#ifndef EDITOR_H
#define EDITOR_H

#include <iostream>

#include "cake.h"
#include "cursor.h"
#include "search.h"
#include "terminal.h"
#include "config/config.h"
#include "net/networking.h"
#include "context.h"

#define CTRL_KEY(k) ((k) & 0x1f)
#define BACKSPACE 127

namespace editor {

    enum EditorMode {
        WRITING,
        PHILOSOPHICAL
    };

    struct UndoState {
        std::string add;
        std::vector<cake::Line> lines;
        Cursor cursor;
    };

    struct State {
        Window window{};
        EditorMode mode;
        Cursor cursor{};
        UndoState undo;
        SearchState search;
    };

    class TextEditor {
    public:
        State state{};
        NetworkBinding networking{};
        std::string path;
        cake::Cake cake;
        config::Config cfg;
        Context* ctx;

        std::vector<UndoState> undoStack;
        std::vector<UndoState> redoStack;

        TextEditor(const config::Config& cfg, Context* ctx);
        ~TextEditor();

        void openFile(const std::string &path);
        void closeFile();

        void render();
        bool handle();

        void undo();
        void redo();

        void pushUndo();

        TextEditor(const TextEditor&) = delete;
        TextEditor& operator=(const TextEditor&) = delete;
    private:
        void drawRows() const;
        void moveCursor() const;
        void drawStatusBar() const;
    };

};

#endif // EDITOR_H
