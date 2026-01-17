#include "tui/menu.h"

#include <unistd.h>

#include "ansi.h"
#include "editor.h"
#include "terminal.h"
#include "tui/tui.h"
#include "global.h"

namespace tui {

Options showMenu(const Window& window) {
    const std::vector<std::string> items = {
        "Open File",
        "Create File",
        "Scratch File",
        "Exit"
    };
    int selected = 0;
    int x, y;
    while (true) {
        clear();
        writeStr(ansi::HIDE_CURSOR);
        writeStr(ansi::CURSOR_HOME);
        drawCenteredBox(window, MENU_WIDTH, MENU_HEIGHT, x, y);
        moveCursor(y + 1, x + (MENU_WIDTH - 12) / 2);
        writeStr("File Options");
        for (size_t i = 0; i < items.size(); ++i) {
            moveCursor(y + 4 + i * 2, x + 4);
            if (static_cast<int>(i) == selected)
                writeStr(ansi::SELECTED);
            writeStr(items[i]);
            writeStr(ansi::NON_SELECTED);
        }
        moveCursor(y + MENU_HEIGHT - 2, x + 4);
        writeStr("↑ ↓  Enter | CTRL-Q for exit");
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1)
            continue;
        if (c == CTRL_KEY('q'))
            return Options::EXIT;
        if (c == '[') {
            selected = (selected - 1 + items.size()) % items.size();
        } else if (c == ']') {
            selected = (selected + 1) % items.size();
        } else if (c == '\n' || c == '\r') {
            return static_cast<Options>(selected);
        }
    }
}

}