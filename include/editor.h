#ifndef EDITOR_H
#define EDITOR_H

#include <termios.h>
#include "cake.h"

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

        void drawRows() const;
        void drawStatusBar() const;
        void moveCursor() const;

    public:
        Editor();
        ~Editor();

        void openFile(const std::string& path);
        void refreshScreen() const;

        int handleWriting(char c);

        bool isWritingMode() const;

        void setMode(MODE mode);

        std::string getPath() const;
        MODE getMode() const;
        cake::Cake &getCake();
        cursor &getCursor();

        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;
    };

}

#endif // EDITOR_H
