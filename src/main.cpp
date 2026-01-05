#include <filesystem>
#include <termios.h>

#include "editor.h"
#include "terminal.h"
#include "events/event.h"
#include "listeners/keyboard_listener.h"
#include "listeners/mode_listener.h"
#include "listeners/movement_listener.h"
#include "listeners/search_listener.h"
#include "tui/alert.h"
#include "tui/menu.h"
#include "tui/prompt.h"

static const std::string CONFIG_PATH = std::string(std::getenv("HOME")) + "/.config/oedipus/config.ini";

void closeEditor(struct termios *term);
void registerListeners(editor::TextEditor& editor);
std::string ask();

int main(int argc, char **argv) {
    std::cout << CONFIG_PATH << std::endl;

    config::Config cfg{};
    try {
        config::load_config(CONFIG_PATH, cfg);
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << '\n';
        return 0;
    }

    struct termios term;
    enableRawMode(&term);
    writeStr("\x1b[?1049h");

    std::string file;
    if (argc != 2) {
        std::string str = ask();
        if (str.empty()) {
            closeEditor(&term);
            return 0;
        }

        file = str;
    } else file = argv[1];

    editor::TextEditor editor(cfg);
    registerListeners(editor);

    editor.openFile(file);
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

void registerListeners(editor::TextEditor& editor) {
    event::EventDispatcher& dispatcher = event::EventDispatcher::instance();

    listener::KeyboardListener keyboardListener(editor);
    dispatcher.registerListener(&keyboardListener);

    listener::MovementListener movementListener(editor);
    dispatcher.registerListener(&movementListener);

    listener::ModeListener modeListener(editor);
    dispatcher.registerListener(&modeListener);

    listener::SearchListener searchListener(editor);
    dispatcher.registerListener(&searchListener);
}

std::string ask() {
    const Window window = windowSize();
    switch (const tui::Options option = tui::showMenu(window)) {
        case tui::Options::EXIT: {
            break;
        }
        case tui::Options::OPEN_FILE: {
            const std::string file = tui::prompt(window, "Open file", "Enter file path:");
            if (std::filesystem::exists(file))
                return file;

            bool flag = false;
            while (!flag)
                flag = tui::alert(window, "File not found", tui::ERROR);

            return std::string{};
        }
        case tui::Options::CREATE_FILE: {
            const std::string file = tui::prompt(window, "Create new file", "Enter file name:");
            if (!std::filesystem::exists(file))
                return file;

            bool flag = false;
            while (!flag)
                flag = tui::alert(window, "File already exists", tui::ERROR);

            return std::string{};
        }
    }

    return std::string{};
}
