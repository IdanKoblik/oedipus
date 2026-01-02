#include "tui/alert.h"

#include <unistd.h>
#include <sys/ioctl.h>
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
        int x, y;
        window::drawCenteredBox(50, 7, x, y);

        window::moveCursor(y+1, x+2);
        printf(alertType == AlertType::ERROR ? "ERROR" : "WARNING");

        window::moveCursor(y+3, x + (50 - msg.size()) / 2);
        printf("%s", msg.c_str());

        window::moveCursor(y+5, x + 13);
        printf("Press any key to continue");

        fflush(stdout);
    }

}
