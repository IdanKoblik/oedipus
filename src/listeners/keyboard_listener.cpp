#include "listeners/keyboard_listener.h"

#include "events/movement_event.h"

#define MOVE_SIZE 4
static const char movementBinds[MOVE_SIZE] = {'h', 'j', 'k', 'l'};

constexpr char modeBind = CTRL_KEY('k');

namespace listener {

    void KeyboardListener::handle(const event::KeyboardEvent &e) {
        if (e.key == modeBind) {
            editor::EditorMode mode = this->editor.state.mode == editor::WRITING
                ? editor::PHILOSOPHICAL
                : editor::WRITING;

            event::ModeEvent event(mode);
            event::EventDispatcher::instance().fire(event);
            return;
        }

        if (this->editor.state.mode) {
            handlePhilosophicalMode(e.key);
            return;
        }
    }

    void KeyboardListener::handlePhilosophicalMode(const char key) {
        for (char bind : movementBinds) {
            if (bind == key) {
                event::MovementEvent event(key);
                event::EventDispatcher::instance().fire(event);
                break;
            }
        }
    }

    void KeyboardListener::handleWritingMode(const char key, editor::TextEditor& editor) {
        Cursor& cursor = editor.state.cursor;
        if (isprint(key)) {
            /*pushUndo();
            redoStack.clear();*/

            editor.cake.insertChar(cursor.x - 1, cursor.y, key);
            cursor.x++;
            return;
        }

        if (key == BACKSPACE || key == '\b') {
            if (cursor.x > 1 || cursor.y > 0) {
                /*pushUndo();
                redoStack.clear();*/
            }

            if (cursor.x > 1) {
                editor.cake.deleteChar(cursor.x - 1, cursor.y);
                cursor.x--;
            }

            if (cursor.y > 0) {
                editor.cake.removeLine(cursor.y);
                cursor.y--;
                cursor.x = editor.cake.lineLength(cursor.y) + 1;
            }
        }

        if (key == '\r') {
            /*pushUndo();
            redoStack.clear();*/

            editor.cake.insertNewLine(cursor.x - 1, cursor.y);
            cursor.y++;
            cursor.x = 1;
        }

        if (key == '\t') {
            /*pushUndo();
            redoStack.clear();*/

            for (int i = 0; i < TAB_SIZE; i++) {
                editor.cake.insertChar(cursor.x - 1, cursor.y, ' ');
                cursor.x++;
            }

            return;
        }
    }

}