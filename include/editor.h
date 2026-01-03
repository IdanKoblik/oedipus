#ifndef EDITOR_H
#define EDITOR_H

#include <map>
#include <termios.h>
#include <unordered_map>

#include "cake.h"
#include "settings/config.h"

namespace editor {

    enum MODE {
        PHILOSOPHICAL,
        WRITING
    };

    struct cursor_t {
        int x;
        int y;
    };

    struct search_t {
        bool active = false;
        std::vector<cursor_t> positions;
        std::map<size_t, std::vector<size_t>> matches;
        std::string target;
        size_t targetSize;
    };

    struct undoState_t {
        std::string add;
        std::vector<cake::Line> lines;
        editor::cursor_t cur;
    };

    class Editor {
    private:
        config::config_t conf;

        struct termios term;
        cake::Cake cake;

        cursor_t cur;
        MODE mode;
        std::string path;
        search_t searchState;

        int screenRows, screenCols;

        std::vector<undoState_t> undoStack;
        std::vector<undoState_t> redoStack;

        void drawRows() const;
        void drawStatusBar() const;
        void moveCursor() const;

        void pushUndo();
        void restoreState(const undoState_t& state);

    public:
        Editor(const config::config_t conf);
        ~Editor();

        void openFile(const std::string& path);
        void refreshScreen() const;

        int handleWriting(char c);

        bool isWritingMode() const;

        void setMode(MODE mode);

        std::string getPath() const;
        MODE getMode() const;
        cake::Cake &getCake();
        cursor_t &getCursor();
        search_t &getSearchState();
        config::config_t getConfig();

        void undo();
        void redo();

        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;
    };

}

#endif // EDITOR_H
