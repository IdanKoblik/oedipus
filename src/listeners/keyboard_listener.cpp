#include "listeners/keyboard_listener.h"

#include "events/movement_event.h"
#include "events/search_event.h"
#include "tui/prompt.h"

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

        if (this->editor.state.mode == editor::PHILOSOPHICAL) {
            handlePhilosophicalMode(e.key, this->editor);
            return;
        }

        handleWritingMode(e.key, this->editor);
    }

    void KeyboardListener::handlePhilosophicalMode(const char key, editor::TextEditor& editor) {
        if (key == '/') {
            const std::string target = tui::prompt(editor.state.window, "Search", "Enter target to search:");

            event::SearchEvent event(target);
            event::EventDispatcher::instance().fire(event);
            return;
        }

        if (key == 'u') {
            editor.undo();
            return;
        }

        if (key == 'r') {
            editor.redo();
            return;
        }

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
            editor.pushUndo();
            editor.redoStack.clear();

            editor.cake.insertChar(cursor.x - 1, cursor.y, key);
            cursor.x++;
            return;
        }

        if (key == BACKSPACE || key == '\b') {
            if (cursor.x > 1 || cursor.y > 0) {
                editor.pushUndo();
                editor.redoStack.clear();
            }

            if (cursor.x > 1) {
                editor.cake.deleteChar(cursor.x - 1, cursor.y);
                cursor.x--;
                return;
            }

            if (cursor.y > 0) {
                editor.cake.removeLine(cursor.y);
                cursor.y--;
                cursor.x = editor.cake.lineLength(cursor.y) + 1;
            }

            return;
        }

        if (key == '\r') {
            editor.pushUndo();
            editor.redoStack.clear();

            editor.cake.insertNewLine(cursor.x - 1, cursor.y);
            cursor.y++;
            cursor.x = 1;

            return;
        }

        if (key == '\t') {
            editor.pushUndo();
            editor.redoStack.clear();

            for (int i = 0; i < TAB_SIZE; i++) {
                editor.cake.insertChar(cursor.x - 1, cursor.y, ' ');
                cursor.x++;
            }

            return;
        }
    }

}