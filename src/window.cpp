#include "window.h"

#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <sys/ioctl.h>

#include "ansi.h"

namespace window {

    void getWindowSize(int &rows, int &cols) {
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
            throw std::runtime_error("ioctl failed");

        rows = ws.ws_row;
        cols = ws.ws_col;
    }

    void drawLine(const std::string& line, const std::vector<size_t>& matches, size_t len) {
        size_t pos = 0;

        for (size_t idx : matches) {
            if (idx < pos)
                continue;

            write(STDOUT_FILENO, line.data() + pos, idx - pos);

            write(STDOUT_FILENO, ansi::SELECTED_CYAN, strlen(ansi::SELECTED_CYAN));
            write(STDOUT_FILENO, line.data() + idx, len);
            write(STDOUT_FILENO, ansi::NON_SELECTED, strlen(ansi::NON_SELECTED));

            pos = idx + len;
        }

        write(STDOUT_FILENO, line.data() + pos, line.size() - pos);

        write(STDOUT_FILENO, ansi::CRLF, strlen(ansi::CRLF));
    }

    void writeStr(const std::string &s) {
        write(STDOUT_FILENO, s.c_str(), s.size());
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

    void drawCenteredBox(int w, int h, int& outX, int& outY) {
        int rows, cols;
        getWindowSize(rows, cols);

        outX = (cols - w) / 2;
        outY = (rows - h) / 2;
        drawBox(outX, outY, w, h);
    }

    void clear() {
        writeStr(ansi::CLEAR);
    }

}
