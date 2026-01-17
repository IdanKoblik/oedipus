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
    constexpr const char* CLEAR = "\033[2J\033[H";

    constexpr const char* SELECTED = "\033[7m";
    constexpr const char* SELECTED_CYAN = "\033[46m";
    constexpr const char* NON_SELECTED = "\033[0m";

    constexpr const char* ENABLE_ALT_BUFFER = "\x1b[?1049h";
    constexpr const char* DISABLE_ALT_BUFFER = "\x1b[?1049l";
}

#endif // ANSI_H