#include <termios.h>
#include <unistd.h>

#include "editor.h"
#include "terminal.h"
#include "events/event.h"
#include "listeners/keyboard_listener.h"
#include "listeners/mode_listener.h"
#include "listeners/movement_listener.h"

void closeEditor(struct termios *term);

int main(int argc, char **argv) {
    if (argc != 2) {
        return 0;
    }

    struct termios term;
    enableRawMode(&term);
    writeStr("\x1b[?1049h");

    editor::TextEditor editor;
    event::EventDispatcher& dispatcher = event::EventDispatcher::instance();

    listener::KeyboardListener keyboardListener(editor);
    dispatcher.registerListener(&keyboardListener);

    listener::MovementListener movementListener(editor);
    dispatcher.registerListener(&movementListener);

    listener::ModeListener modeListener(editor);
    dispatcher.registerListener(&modeListener);

    editor.openFile(argv[1]);
    while (true) {
        editor.render();

        if (!editor.handle())
            break;
    }

    closeEditor(&term);
    return 0;
}

void closeEditor(struct termios *term) {
    writeStr("\x1b[?1049l");
    disableRawMode(term);
}
