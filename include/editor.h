#ifndef EDITOR_H
#define EDITOR_H

#include <iostream>

#include "cake.h"
#include "cursor.h"
#include "terminal.h"

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

        TextEditor(const TextEditor&) = delete;
        TextEditor& operator=(const TextEditor&) = delete;
    private:
        void drawRows() const;
        void moveCursor() const;
        void drawStatusBar() const;
    };

};

#endif // EDITOR_H
