#include <unistd.h>
#include <iostream>

#include "editor.h"
#include "terminal.h"
#include "events/event.h"
#include "events/keyboard_event.h"
#include "listeners/keyboard_listener.h"
#include "listeners/mode_listener.h"
#include "listeners/move_listener.h"

int main(int argc, char* argv[]) {
    try {
        if (argc < 2)
            throw std::runtime_error("No file provided");

        editor::Editor editor;
        event::EventDispatcher dispatcher;

        editor.openFile(argv[1]);

        listener::KeyboardListener KeyboardListener(editor, dispatcher);
        listener::ModeListener modeListener(editor);
        listener::MoveListener moveListener(editor);

        dispatcher.registerListener(&KeyboardListener);
        dispatcher.registerListener(&modeListener);
        dispatcher.registerListener(&moveListener);

        while (true) {
            editor.refreshScreen();

            char c;
            if (read(STDIN_FILENO, &c, 1) != 1)
                continue;

            if (c == CTRL_KEY('q'))
                break;

            event::KeyboardEvent e(c);
            dispatcher.fire(e);

            if (editor.isWritingMode())
                editor.handleWriting(c);
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << '\n';
        return 1;
    }
}
