#ifndef ANSI_H
#define ANSI_H

namespace ansi {
    constexpr const char* HIDE_CURSOR = "\x1b[?25l";
    constexpr const char* SHOW_CURSOR = "\x1b[?25h";

    constexpr const char* CURSOR_HOME = "\x1b[H";

    constexpr const char* CLEAR_LINE = "\x1b[K";

    constexpr const char* INVERT_COLORS = "\x1b[7m";
    constexpr const char* RESET_ATTRS   = "\x1b[m";

    constexpr const char* CRLF = "\r\n";

}

#endif // ANSI_H