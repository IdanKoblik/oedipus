#include "tui/alert.h"

#include <unistd.h>
#include <sys/ioctl.h>
#include "window.h"

namespace tui {

    std::string parseType(AlertType type) {
        switch (type) {
            case ERROR: return "ERROR";
            case WARNING: return "WARNING";
            case NOTIFICATION: return "NOTIFICATION";
            default: return "UNKNOWN";
        }
    }

    bool alert(const std::string &msg, AlertType alertType) {
        window::clear();

        int x, y;
        window::drawCenteredBox(50, 7, x, y);

        window::moveCursor(y + 1, x + 2);
        window::writeStr(parseType(alertType));

        window::moveCursor(y + 3, x + (50 - msg.size()) / 2);
        window::writeStr(msg);

        window::moveCursor(y + 5, x + 13);
        window::writeStr("Press any key to continue");

        char c;
        if (read(STDIN_FILENO, &c, 1) <= 0)
            return false;

        return true;
    }

}
