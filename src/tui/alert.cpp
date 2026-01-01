#include "tui/alert.h"

#include <curses.h>

#include "window.h"

namespace tui {

    std::string parseType(AlertType type) {
        switch (type) {
            case AlertType::ERROR: return "ERROR";
            case AlertType::WARNING: return "WARNING";
            default: return "UNKNOWN";
        }
    }

    void alert(const std::string &msg, AlertType alertType) {
        WINDOW *window = centerWindow(ALERT_HEIGHT, ALERT_WIDTH);
        box(window, 0, 0);

        const std::string type = parseType(alertType);
        mvwprintw(window, 1, 2, type.c_str());
        mvwhline(window, 2, 1, ACS_HLINE, ALERT_WIDTH - 2);

        mvwprintw(window, 4, (ALERT_WIDTH - msg.size()) / 2, "%s", msg.c_str());
        mvwprintw(window, ALERT_H - 2, (ALERT_WIDTH - 24) / 2, "Press any key to continue");

        wrefresh(window);
        wgetch(window);
        delwin(window);
    }

}
