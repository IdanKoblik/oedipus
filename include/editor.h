#ifndef EDITOR_H
#define EDITOR_H

#include <iostream>

#include "cake.h"
#include "cursor.h"
#include "terminal.h"

#define CTRL_KEY(k) ((k) & 0x1f)
#define BACKSPACE 127
#define TAB_SIZE 4

namespace editor {

    enum EditorMode {
        WRITING,
        PHILOSOPHICAL
    };

    struct State {
        Window window;
        EditorMode mode;
        Cursor cursor;
    };

    class TextEditor {
    public:
        State state{};
        std::string path;
        cake::Cake cake;

        TextEditor();
        ~TextEditor();

        void openFile(const std::string &path);
        void closeFile();

        void render();

        bool handle();

        TextEditor(const TextEditor&) = delete;
        TextEditor& operator=(const TextEditor&) = delete;
    private:
        void drawRows() const;
        void moveCursor() const;
        void drawStatusBar() const;
    };

};

#endif // EDITOR_H
