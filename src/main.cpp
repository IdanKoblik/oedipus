#include <fstream>
#include <unistd.h>
#include <iostream>
#include <ncurses.h>
#include "editor.h"
#include "terminal.h"
#include "events/event.h"
#include "events/keyboard_event.h"
#include "listeners/keyboard_listener.h"
#include "listeners/mode_listener.h"
#include "listeners/move_listener.h"
#include "tui/menu.h"

void startEditor(const std::string &filePath);

int main(int argc, char* argv[]) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
    curs_set(0);

    if (argc > 2) {
        startEditor(argv[1]);
        endwin();

        return 0;
    }

    const tui::Options option = tui::showMenu();
    refresh();

    switch (option) {
        case tui::Options::EXIT: {
            endwin();
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
    }

    endwin();
}