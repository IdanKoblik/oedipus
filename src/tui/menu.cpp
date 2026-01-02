#include "tui/menu.h"

#include <cstring>
#include <unistd.h>
#include <string>
#include <vector>

#include "ansi.h"
#include "window.h"
#include "terminal.h"

namespace tui {

    Options showMenu() {
        std::vector<std::string> items = {
            "Open File",
            "Create File",
            "Scratch File",
            "Exit"
        };

        int selected = 0;
        int x, y;

        while (true) {
            window::clear();
            write(STDOUT_FILENO, ansi::HIDE_CURSOR, strlen(ansi::HIDE_CURSOR));
            write(STDOUT_FILENO, ansi::CURSOR_HOME, strlen(ansi::CURSOR_HOME));

            window::drawCenteredBox(MENU_WIDTH, MENU_HEIGHT, x, y);

            window::moveCursor(y + 1, x + (MENU_WIDTH - 12) / 2);
            window::writeStr("File Options");

            for (size_t i = 0; i < items.size(); ++i) {
                window::moveCursor(y + 4 + i * 2, x + 4);

                if (static_cast<int>(i) == selected)
                    window::writeStr(ansi::SELECTED);

                window::writeStr(items[i]);
                window::writeStr(ansi::NON_SELECTED);
            }

            window::moveCursor(y + MENU_HEIGHT - 2, x + 4);
            window::writeStr("↑ ↓  Enter | q for exit");

            char c;
            if (read(STDIN_FILENO, &c, 1) != 1)
                continue;

            if (c == 'q')
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

    std::string prompt(const std::string& title, const std::string& msg) {
        int x, y;
        window::clear();
        window::drawCenteredBox(PROMPT_WIDTH, PROMPT_HEIGHT, x, y);

        window::moveCursor(y + 1, x + 2);
        window::writeStr(title);

        window::moveCursor(y + 3, x + 2);
        window::writeStr(msg);

        int inputX = x + 2;
        int inputY = y + 5;
        window::drawBox(inputX, inputY, PROMPT_WIDTH - 4, 3);

        window::moveCursor(inputY + 1, inputX + 1);

        std::string input;

        while (true) {
            char c;
            if (read(STDIN_FILENO, &c, 1) != 1)
                continue;

            if (c == '\n' || c == '\r') {
                break;
            }

            else if (c == 127 || c == 8) { // backspace
                if (!input.empty()) {
                    input.pop_back();
                    window::moveCursor(inputY + 1, inputX + 1);
                    window::writeStr(std::string(PROMPT_WIDTH - 6, ' '));
                    window::moveCursor(inputY + 1, inputX + 1);
                    window::writeStr(input);
                }
            }
            else if (c >= 32 && c <= 126) {
                if (static_cast<int>(input.size()) < PROMPT_WIDTH - 6) {
                    input.push_back(c);
                    window::writeStr(std::string(1, c));
                }
            }
        }

        return input;
    }

}
