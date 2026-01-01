#include "window.h"

#include <stdexcept>
#include <unistd.h>
#include <sys/ioctl.h>

void getWindowSize(int &rows, int &cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
        throw std::runtime_error("ioctl failed");

    rows = ws.ws_row;
    cols = ws.ws_col;
}

WINDOW* centerWindow(int h, int w) {
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    return newwin(h, w, y, x);
}