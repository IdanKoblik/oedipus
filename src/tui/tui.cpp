#include "tui/tui.h"

#include <cstring>
#include <unistd.h>
#include <vector>

#include "ansi.h"
#include "terminal.h"

namespace tui {

void drawLine(const std::string& line, const std::vector<size_t>& matches, size_t len) {
    size_t pos = 0;
    for (size_t idx : matches) {
        if (idx < pos)
            continue;
        write(STDOUT_FILENO, line.data() + pos, idx - pos);
        writeStr(ansi::SELECTED_CYAN);
        write(STDOUT_FILENO, line.data() + idx, len);
        writeStr(ansi::NON_SELECTED);
        pos = idx + len;
    }
    write(STDOUT_FILENO, line.data() + pos, line.size() - pos);
    writeStr(ansi::CRLF);
}

void moveCursor(int r, int c) {
    writeStr("\033[" + std::to_string(r) + ";" + std::to_string(c) + "H");
}

void drawBox(int x, int y, int w, int h) {
    moveCursor(y, x);
    writeStr(TL);
    for (int i = 0; i < w - 2; i++)
        writeStr(H);
    writeStr(TR);
    for (int i = 1; i < h - 1; i++) {
        moveCursor(y + i, x);
        writeStr(V);
        moveCursor(y + i, x + w - 1);
        writeStr(V);
    }
    moveCursor(y + h - 1, x);
    writeStr(BL);
    for (int i = 0; i < w - 2; i++)
        writeStr(H);
    writeStr(BR);
}

void drawCenteredBox(const Window& window, int w, int h, int& outX, int& outY) {
    outX = (window.cols - w) / 2;
    outY = (window.rows - h) / 2;
    drawBox(outX, outY, w, h);
}

}