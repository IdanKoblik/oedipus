#include <termios.h>
#include <unistd.h>

#include "editor.h"
#include "terminal.h"

void closeEditor(struct termios *term);

int main(int argc, char **argv) {
    struct termios term;
    enableRawMode(&term);
    writeStr("\x1b[?1049h");

    editor::TextEditor editor;
    editor.openFile(argv[1]);
    while (true) {
        editor.render();

        char c;
        if (read(STDIN_FILENO, &c, 1) == -1)
            break;

        if (c == 'q')
            break;
    }

    closeEditor(&term);
    return 0;
}

void closeEditor(struct termios *term) {
    writeStr("\x1b[?1049l");
    disableRawMode(term);
}
