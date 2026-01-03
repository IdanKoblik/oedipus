#include <cstring>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include "editor.h"
#include "terminal.h"
#include "window.h"
#include "events/event.h"
#include "events/keyboard_event.h"
#include "listeners/keyboard_listener.h"
#include "listeners/mode_listener.h"
#include "listeners/move_listener.h"
#include "listeners/search_listener.h"
#include "tui/menu.h"

void startEditor(const std::string &filePath);

int main(int argc, char* argv[]) {
    if (argc == 2) {
        startEditor(argv[1]);
        return 0;
    }

    struct termios term;
    if (terminal::enableRawMode(&term) == -1)
        throw std::runtime_error(std::strerror(errno));

    switch (const tui::Options option = tui::showMenu()) {
        case tui::Options::EXIT: {
            break;
        }
        case tui::Options::OPEN_FILE: {
            const std::string file = tui::prompt("Open file", "Enter file path:");

            startEditor(file);
            break;
        }
        case tui::Options::CREATE_FILE: {
            const std::string file = tui::prompt("Create new file", "Enter file name:");

            std::ofstream outfile (file);
            outfile.close();

            startEditor(file);
            break;
        }
    }

    terminal::disableRawMode(&term);
    return 0;
}

void startEditor(const std::string &filePath) {
    try {
        editor::Editor editor;
        event::EventDispatcher dispatcher;

        editor.openFile(filePath);

        listener::KeyboardListener KeyboardListener(editor, dispatcher);
        listener::ModeListener modeListener(editor);
        listener::MoveListener moveListener(editor);
        listener::SearchListener search_listener(editor);

        dispatcher.registerListener(&KeyboardListener);
        dispatcher.registerListener(&modeListener);
        dispatcher.registerListener(&moveListener);
        dispatcher.registerListener(&search_listener);

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
    }
}