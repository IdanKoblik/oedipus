#ifndef EDITOR_H
#define EDITOR_H

#include <termios.h>
#include "cake.h"

#define CTRL_KEY(k) ((k) & 0x1f)
#define BACKSPACE 127

namespace editor {

    enum MODE {
        PHILOSOPHICAL,
        WRITING
    };

    struct cursor {
        int x;
        int y;
    };

    class Editor {
    private:
        struct termios term;
        cake::Cake cake;

        cursor cur;
        MODE mode;
        std::string path;

        int screenRows, screenCols;
        int padding;

        void drawRows() const;
        void drawStatusBar() const;
        void moveCursor() const;

    public:
        Editor();
        ~Editor();

        void openFile(const std::string& path);
        void refreshScreen() const;

        int handleWriting(char c);
        int handleCursor(char c);

        bool isWritingMode() const;

        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;
    };

}

#endif // EDITOR_H
