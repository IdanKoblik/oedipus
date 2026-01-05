#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>
#include <string>

struct Window {
    size_t rows;
    size_t cols;
};

int enableRawMode(struct termios *term);

void disableRawMode(struct termios *term);

void writeStr(const std::string &s);

void clear();

Window windowSize();

#endif // TERMINAL_H
