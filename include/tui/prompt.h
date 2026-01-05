#ifndef PROMPT_H
#define PROMPT_H

#include <string>
#include "terminal.h"

#define PROMPT_HEIGHT 11
#define PROMPT_WIDTH 70

namespace tui {

    std::string prompt(const Window& window, const std::string& title, const std::string& msg);

}

#endif // PROMPT_H