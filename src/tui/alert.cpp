#include "tui/alert.h"

#include <unistd.h>

#include "terminal.h"
#include "tui/tui.h"

namespace tui {

    std::string parseType(AlertType type) {
        switch (type) {
            case ERROR: return "ERROR";
            case WARNING: return "WARNING";
            case INFO: return "INFO";
            default: return "UNKNOWN";
        }
    }

    bool alert(const Window& window, const std::string &msg, AlertType alertType) {
        clear();

        int x, y;
        drawCenteredBox(window, 50, 7, x, y);

        moveCursor(y + 1, x + 2);
        writeStr(parseType(alertType));

        moveCursor(y + 3, x + (50 - msg.size()) / 2);
        writeStr(msg);

        moveCursor(y + 5, x + 13);
        writeStr("Press any key to continue");

        char c;
        if (read(STDIN_FILENO, &c, 1) <= 0)
            return false;

        return true;
    }
}
