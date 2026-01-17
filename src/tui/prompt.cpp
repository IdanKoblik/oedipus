#include "tui/prompt.h"

#include <unistd.h>
#include "editor.h"
#include "terminal.h"
#include "tui/tui.h"
#include "global.h"

namespace tui {

std::string prompt(const Window& window, const std::string& title, const std::string& msg) {
    int x, y;
    clear();
    drawCenteredBox(window, PROMPT_WIDTH, PROMPT_HEIGHT, x, y);
    moveCursor(y + 1, x + 2);
    writeStr(title);
    moveCursor(y + 3, x + 2);
    writeStr(msg);
    int inputX = x + 2;
    int inputY = y + 5;
    drawBox(inputX, inputY, PROMPT_WIDTH - 4, 3);
    moveCursor(inputY + 1, inputX + 1);
    std::string input;
    while (true) {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1)
            continue;
        if (c == '\n' || c == '\r')
            break;
        if (c == BACKSPACE || c == 8) { // backspace
            if (!input.empty()) {
                input.pop_back();
                moveCursor(inputY + 1, inputX + 1);
                writeStr(std::string(PROMPT_WIDTH - 6, ' '));
                moveCursor(inputY + 1, inputX + 1);
                writeStr(input);
            }
        }
        else if (c >= 32 && c <= 126) {
            if (static_cast<int>(input.size()) < PROMPT_WIDTH - 6) {
                input.push_back(c);
                writeStr(std::string(1, c));
            }
        }
    }
    return input;
}

}