#ifndef MENU_H
#define MENU_H

#include <string>
#include <unistd.h>
#include <vector>

#include "ansi.h"
#include "tui.h"

#define MENU_HEIGHT 15
#define MENU_WIDTH 50

struct Window;

namespace tui {

    enum class FileOptions {
        OPEN_FILE = 0,
        CREATE_FILE = 1,
        CONNECT = 2,
        EXIT = 3
    };

    enum class CwmOptions {
        HOST = 0,
        DISCONNECT = 1,
    };

    template<typename Enum>
    Enum showMenu(
        const Window& window,
        const std::string& title,
        const std::vector<std::pair<Enum, std::string>>& items
    ) {
        int selected = 0;
        int x, y;

        while (true) {
            clear();
            writeStr(ansi::HIDE_CURSOR);
            writeStr(ansi::CURSOR_HOME);

            drawCenteredBox(window, MENU_WIDTH, MENU_HEIGHT, x, y);

            moveCursor(y + 1, x + (MENU_WIDTH - title.size()) / 2);
            writeStr(title);

            for (size_t i = 0; i < items.size(); ++i) {
                moveCursor(y + 4 + i * 2, x + 4);

                if (static_cast<int>(i) == selected)
                    writeStr(ansi::SELECTED);

                writeStr(items[i].second);
                writeStr(ansi::NON_SELECTED);
            }

            char c;
            if (read(STDIN_FILENO, &c, 1) != 1)
                continue;

            if (c == CTRL_KEY('q'))
                return items.back().first;

            if (c == '[')
                selected = (selected - 1 + items.size()) % items.size();
            else if (c == ']')
                selected = (selected + 1) % items.size();
            else if (c == '\n' || c == '\r')
                return items[selected].first;
        }
    }

}

#endif // MENU_H