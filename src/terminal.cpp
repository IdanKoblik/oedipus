#include "terminal.h"

#include <cstring>

namespace terminal {

    int enableRawMode(struct termios *term) {
        if (tcgetattr(STDIN_FILENO, term) == -1) {
            return -1;
        }

        struct termios raw = *term;
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_cc[VMIN]  = 0;
        raw.c_cc[VTIME] = 1;

        return tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }

    void disableRawMode(struct termios *term) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, term);
    }

}
