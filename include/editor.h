#ifndef EDITOR_H
#define EDITOR_H

#include <fstream>
#include <termios.h>
#include <vector>
#include "terminal.h"

#define BACKSPACE 127

namespace editor {

typedef enum {
    PHILOSOPHICAL,
    WRITING
} MODE;

inline std::string toString(MODE mode) {
    switch (mode) {
        case PHILOSOPHICAL: return "PHILOSOPHICAL";
        case WRITING: return "WRITING";
        default: return "Unknown";
    }
}

typedef struct {
    int x;
    int y;
} cursor;

class Editor {
private:
    struct termios term;
    std::vector<std::string> rows;
    cursor cur;
    MODE mode;
    std::string path;

    int writeStartX = 0;
    std::string writeBuffer;

    int screenCols, screenRows;
    int padding;

    void drawRows() const;
    void drawStatusBar() const;
    void moveCursor() const;

public:
    Editor();
    ~Editor();

    void openFile(const std::string &path);

    void refreshScreen() const;
    int handleWriting(char c);

    int handleCursor(char c);
    cursor *getCursor();

    Editor(const Editor&) = delete;
    Editor& operator=(const Editor&) = delete;
};

}

#endif //EDITOR_H
