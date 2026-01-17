#ifndef KEYBOARD_HANDLER
#define KEYBOARD_HANDLER

#include "cursor.h"
#include "structs/piece_table.h"
#include "algorithem/search.h"
#include "editor.h"

void handleWritingMode(char c, editor::TextEditor* editor);

void handlePhilosophicalMode(char c, editor::TextEditor* editor);

#endif //KEYBOARD_HANDLER