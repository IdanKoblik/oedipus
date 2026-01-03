#ifndef EDITOR_H
#define EDITOR_H

#include <map>
#include <termios.h>
#include <unordered_map>

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

    struct search {
        bool active = false;
        std::vector<cursor> positions;
        std::map<size_t, std::vector<size_t>> matches;
        std::string target;
        size_t targetSize;
    };

    class Editor {
    private:
        struct termios term;
        cake::Cake cake;

        cursor cur;
        MODE mode;
        std::string path;
        search searchState;

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
        search &getSearchState();

        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;
    };

}

#endif // EDITOR_H
