#ifndef MENU_H
#define MENU_H

#include <string>

#define MENU_HEIGHT 15
#define MENU_WIDTH 50

struct Window;

namespace tui {

enum class Options {
    OPEN_FILE = 0,
    CREATE_FILE = 1,
    EXIT = 2
};

Options showMenu(const Window& window);

}

#endif // MENU_H