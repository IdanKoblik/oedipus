#ifndef ALERT_H
#define ALERT_H

#include <string>

#include "terminal.h"

namespace tui {

    enum AlertType {
        ERROR,
        WARNING,
        INFO,
        UNKNOWN
    };

    std::string parseType(AlertType type);

    bool alert(const Window& window, const std::string &msg, AlertType alertType);

}

#endif // ALERT_H