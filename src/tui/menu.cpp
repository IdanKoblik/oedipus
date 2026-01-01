#include "tui/menu.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#include "window.h"

namespace tui {

    Options showMenu() {
        std::vector<std::string> items = {
            "Open File",
            "Create File",
            "Scratch File",
            "Exit"
        };

        WINDOW* win = centerWindow(MENU_HEIGHT, MENU_WIDTH);
        keypad(win, true);

        int selected = 0;

        while (true) {
            box(win, 0, 0);
            mvwprintw(win, 1, (MENU_WIDTH - 20) / 2, "File Options");
            mvwhline(win, 2, 1, ACS_HLINE, MENU_WIDTH - 2);

            for (size_t i = 0; i < items.size(); ++i) {
                int y = 4 + i * 2;

                if (static_cast<int>(i) == selected)
                    wattron(win, A_REVERSE);

                mvwprintw(win, y, 4, items[i].c_str());

                wattroff(win, A_REVERSE);
            }

            mvwprintw(win, MENU_HEIGHT - 2, 4, "Use ↑ ↓  Enter to select");

            wrefresh(win);

            int ch = wgetch(win);
            switch (ch) {
                case KEY_UP:
                    selected = (selected - 1 + items.size()) % items.size();
                    break;
                case KEY_DOWN:
                    selected = (selected + 1) % items.size();
                    break;
                case '\n':
                    delwin(win);
                    return static_cast<Options>(selected);
            }
        }

    }

    std::string prompt(const std::string &title, const std::string &prompt) {
        WINDOW* dialog = centerWindow(PROMPT_HEIGHT, PROMPT_WIDTH);
        box(dialog, 0, 0);

        mvwprintw(dialog, 1, 2, "%s", title.c_str());
        mvwhline(dialog, 2, 1, ACS_HLINE, PROMPT_WIDTH - 2);
        mvwprintw(dialog, 4, 2, prompt.c_str());

        int box_w = PROMPT_WIDTH - 6;
        WINDOW* textbox = derwin(dialog, 3, box_w, 6, 3);
        box(textbox, 0, 0);
        wrefresh(dialog);
        wrefresh(textbox);

        echo();
        curs_set(1);

        char buf[256]{};
        mvwgetnstr(textbox, 1, 1, buf, 255);

        noecho();
        curs_set(0);

        delwin(textbox);
        delwin(dialog);

        return std::string(buf);
    }

}
