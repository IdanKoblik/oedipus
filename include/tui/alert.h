#ifndef ALERT_H
#define ALERT_H

#include <string>

#define ALERT_HEIGHT 9
#define ALERT_WIDTH 60

namespace tui {

    enum AlertType {
        ERROR,
        WARNING,
        NOTIFICATION,
    };

    bool alert(const std::string &msg, AlertType alertType);

}

#endif // ALERT_H