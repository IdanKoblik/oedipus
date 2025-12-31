#include "editor.h"

#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <cctype>

#include "ansi.h"
#include "window.h"
#include "terminal.h"

namespace editor {

    void Editor::drawRows() const {
        for (int y = 0; y < screenRows - 1; y++) {
            write(STDOUT_FILENO, ansi::CLEAR_LINE, strlen(ansi::CLEAR_LINE));

            if (y < static_cast<int>(cake.lineCount())) {
                std::string content = cake.renderLine(y);

                int numPad = padding - std::to_string(y + 1).length();
                std::string line(numPad, ' ');
                line += std::to_string(y + 1);
                line += " ";
                line += content;

                write(STDOUT_FILENO, line.c_str(), line.size());
            }

            write(STDOUT_FILENO, ansi::CRLF, 2);
        }
    }

    void Editor::drawStatusBar() const {
        write(STDOUT_FILENO, ansi::INVERT_COLORS, strlen(ansi::INVERT_COLORS));

        std::string status =
                " oedipus | " +
                std::string(mode == WRITING ? "WRITING" : "PHILOSOPHICAL") +
                " | Ctrl-K toggle | Ctrl-Q quit ";

        if (static_cast<int>(status.size()) > screenCols)
            status.resize(screenCols);

        write(STDOUT_FILENO, status.c_str(), status.size());

        while (static_cast<int>(status.size()) < screenCols) {
            write(STDOUT_FILENO, " ", 1);
            status.push_back(' ');
        }

        write(STDOUT_FILENO, ansi::RESET_ATTRS, strlen(ansi::RESET_ATTRS));
    }

    void Editor::moveCursor() const {
        char buf[32];

        // +1 for the immutable space after line number
        int screenX = padding + 1 + cur.x;
        int screenY = cur.y + 1;

        snprintf(buf, sizeof(buf), "\x1b[%d;%dH", screenY, screenX);
        write(STDOUT_FILENO, buf, strlen(buf));
    }

    Editor::Editor() {
        mode = PHILOSOPHICAL;

        if (terminal::enableRawMode(&term) == -1)
            throw std::runtime_error(std::strerror(errno));

        getWindowSize(screenRows, screenCols);
        write(STDOUT_FILENO, ansi::SHOW_CURSOR, strlen(ansi::SHOW_CURSOR));
    }

    Editor::~Editor() {
        write(STDOUT_FILENO, ansi::SHOW_CURSOR, strlen(ansi::SHOW_CURSOR));
        terminal::disableRawMode(&term);
    }

    void Editor::openFile(const std::string& p) {
        path = p;
        cake.loadFromFile(p);

        padding = std::to_string(cake.lineCount()).length();
        cur = { 1, 0 };
    }

    void Editor::refreshScreen() const {
        write(STDOUT_FILENO, ansi::HIDE_CURSOR, strlen(ansi::HIDE_CURSOR));
        write(STDOUT_FILENO, ansi::CURSOR_HOME, strlen(ansi::CURSOR_HOME));

        drawRows();
        drawStatusBar();
        moveCursor();

        write(STDOUT_FILENO, ansi::SHOW_CURSOR, strlen(ansi::SHOW_CURSOR));
    }

    int Editor::handleWriting(char c) {
        if (mode != WRITING)
            return 0;

        if (isprint(c)) {
            cake.insertChar(cur.x, cur.y, c);
            cur.x++;
            return 0;
        }

        if (c == BACKSPACE || c == '\b') {
            if (cur.x > 1) {
                cake.deleteChar(cur.x - 1, cur.y);
                cur.x--;
            }
            return 0;
        }

        if (c == '\r') {
            cake.insertNewLine(cur.x, cur.y);
            cur.y++;
            cur.x = 1;
            return 0;
        }

        return 0;
    }

    bool Editor::isWritingMode() const {
        return mode == WRITING;
    }

    void Editor::setMode(const MODE mode) {
        this->mode = mode;
    }

    std::string Editor::getPath() const {
        return path;
    }

    MODE Editor::getMode() const {
        return mode;
    }

    cake::Cake &Editor::getCake() {
        return cake;
    }

    cursor &Editor::getCursor() {
        return cur;
    }

}
