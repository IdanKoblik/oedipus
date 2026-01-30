#include "tui/alert.h"

#include <unistd.h>

#include "terminal.h"
#include "tui/tui.h"

namespace tui {

std::string parseType(AlertType type) {
    switch (type) {
        case AlertType::ERROR: return "ERROR";
        case AlertType::WARNING: return "WARNING";
        case AlertType::INFO: return "INFO";
        default: return "UNKNOWN";
    }
}

void alert(const Window& window, const std::string &msg, AlertType alertType) {
    char c;

    clear();
    int x, y;

    while (true) {
        drawCenteredBox(window, 50, 7, x, y);
        moveCursor(y + 1, x + 2);
        writeStr(parseType(alertType));
        moveCursor(y + 3, x + (50 - msg.size()) / 2);
        writeStr(msg);
        moveCursor(y + 5, x + 13);
        writeStr("Press any key to continue");


        if (read(STDIN_FILENO, &c, 1) > 0)
            break;
    }
}

}