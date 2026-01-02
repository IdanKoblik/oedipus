#ifndef MENU_H
#define MENU_H

#include <string>

#define MENU_HEIGHT 15
#define MENU_WIDTH 50

#define PROMPT_HEIGHT 11
#define PROMPT_WIDTH 70

namespace tui {

    enum class Options {
        OPEN_FILE = 0,
        CREATE_FILE = 1,
        EXIT = 2
    };

    Options showMenu();

    std::string prompt(const std::string &title, const std::string &prompt);

}

#endif // MENU_H