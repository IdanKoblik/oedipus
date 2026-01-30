#ifndef ALERT_H
#define ALERT_H

#include <string>

#include "terminal.h"

namespace tui {

enum class AlertType {
    ERROR,
    WARNING,
    INFO,
    UNKNOWN
};

std::string parseType(AlertType type);

void alert(const Window& window, const std::string &msg, AlertType alertType);

}

#endif // ALERT_H