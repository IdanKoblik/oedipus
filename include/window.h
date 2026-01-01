#ifndef WINDOW_H
#define WINDOW_H

#include <ncurses.h>

void getWindowSize(int &rows, int &cols);

WINDOW *centerWindow(int h, int w);

#endif //WINDOW_H