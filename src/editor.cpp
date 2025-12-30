#include "editor.h"

#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <unistd.h>

#include "ansi.h"
#include "window.h"

namespace editor {

Editor::Editor() {
    mode = MODE::PHILOSOPHICAL;

    if (terminal::enableRawMode(&term) == -1)
        throw std::runtime_error(std::strerror(errno));

    getWindowSize(screenRows, screenCols);
    write(STDOUT_FILENO, ansi::SHOW_CURSOR, strlen(ansi::SHOW_CURSOR));
}

Editor::~Editor() {
    write(STDOUT_FILENO, ansi::SHOW_CURSOR, strlen(ansi::SHOW_CURSOR));
    terminal::disableRawMode(&term);
}

void Editor::openFile(const std::string& path) {
    if (!std::filesystem::exists(path))
        throw std::runtime_error("File does not exist");

    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error(std::strerror(errno));

    this->path = path;
    rows.clear();

    std::string line;
    while (std::getline(file, line))
        rows.push_back(line);

    padding = std::to_string(rows.size()).length() + 1;
    cur = { padding, 0 };
}

void Editor::drawRows() const {
    for (int y = 0; y < screenRows - 1; y++) {
        write(STDOUT_FILENO, ansi::CLEAR_LINE, strlen(ansi::CLEAR_LINE));

        if (y < static_cast<int>(rows.size())) {
            std::string content = rows[y];

            if (mode == MODE::WRITING && y == cur.y) {
                int insertPos = writeStartX - padding;
                if (insertPos < 0) insertPos = 0;
                if (insertPos > (int)content.size())
                    insertPos = content.size();

                content.insert(insertPos, writeBuffer);
            }

            int numPad = padding - std::to_string(y + 1).length();
            std::string line(numPad, ' ');
            line += std::to_string(y + 1) + " " + content;

            write(STDOUT_FILENO, line.c_str(), line.size());
        }

        write(STDOUT_FILENO, ansi::CRLF, 2);
    }
}

void Editor::drawStatusBar() const {
    write(STDOUT_FILENO, ansi::INVERT_COLORS, strlen(ansi::INVERT_COLORS));

    std::string status = " oedipus | " + toString(mode) + " | Ctrl-M toggle | Ctrl-Q quit ";

    if (static_cast<int>(status.size()) > screenCols)
        status.resize(screenCols);

    write(STDOUT_FILENO, status.c_str(), status.size());

    while (static_cast<int>(status.size()) < screenCols) {
        write(STDOUT_FILENO, " ", 1);
        status.push_back(' ');
    }

    write(STDOUT_FILENO, ansi::RESET_ATTRS, strlen(ansi::RESET_ATTRS));
}

void Editor::refreshScreen() const {
    write(STDOUT_FILENO, ansi::HIDE_CURSOR, strlen(ansi::HIDE_CURSOR));
    write(STDOUT_FILENO, ansi::CURSOR_HOME, strlen(ansi::CURSOR_HOME));

    drawRows();
    drawStatusBar();
    moveCursor();

    write(STDOUT_FILENO, ansi::SHOW_CURSOR, strlen(ansi::SHOW_CURSOR));
}

void Editor::moveCursor() const {
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "\x1b[%d;%dH", cur.y + 1, cur.x + 1);
    write(STDOUT_FILENO, buf, len);
}

int Editor::handleWriting(char c) {
    if (mode != MODE::WRITING)
        return 1;

    if (c == CTRL_KEY('m')) {
        mode = MODE::PHILOSOPHICAL;

        std::string& line = rows[cur.y];
        int insertPos = writeStartX - padding;

        if (insertPos < 0) insertPos = 0;
        if (insertPos > (int)line.size())
            insertPos = line.size();

        line.insert(insertPos, writeBuffer);
        writeBuffer.clear();

        std::ofstream file(path);
        for (const auto& r : rows)
            file << r << '\n';

        return 0;
    }

    if (isprint(c)) {
        writeBuffer.push_back(c);
        cur.x++;
    }

    if ((c == BACKSPACE || c == '\b') && (!writeBuffer.empty() && cur.x > 0)) {
        writeBuffer.pop_back();
        cur.x--;
    }

    return 0;
}

int Editor::handleCursor(char c) {
    if (mode != MODE::PHILOSOPHICAL)
        return 0;

    if (c == CTRL_KEY('q'))
        return 1;

    if (c == CTRL_KEY('m')) {
        mode = MODE::WRITING;
        writeBuffer.clear();
        writeStartX = cur.x;
        return 0;
    }

    switch (c) {
        case 'h': cur.x--; break;
        case 'l': cur.x++; break;
        case 'k': cur.y--; break;
        case 'j': cur.y++; break;
    }

    if (cur.x < padding)
        cur.x = padding;
    if (cur.y < 0)
        cur.y = 0;
    if (cur.y >= (int)rows.size())
        cur.y = rows.size() - 1;

    return 0;
}

} // namespace editor