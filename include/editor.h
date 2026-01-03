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

    struct UndoState {
        std::string add;
        std::vector<cake::Line> lines;
        editor::cursor cur;
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

        std::vector<UndoState> undoStack;
        std::vector<UndoState> redoStack;

        void drawRows() const;
        void drawStatusBar() const;
        void moveCursor() const;

        void pushUndo();
        void restoreState(const UndoState& state);

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

        void undo();
        void redo();

        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;
    };

}

#endif // EDITOR_H
