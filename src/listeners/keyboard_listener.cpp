#include "listeners/keyboard_listener.h"

#include <cstring>

#include "kmp.h"
#include "terminal.h"
#include "events/move_event.h"
#include "events/search_event.h"
#include "tui/alert.h"
#include "tui/menu.h"

#define MOVE_CHARS_SIZE 5
const char moveChars[MOVE_CHARS_SIZE] = { 'h', 'j', 'k', 'l', 'n' };

namespace listener {

    static void handlePhilosophicalMode(const event::KeyboardEvent& e, editor::Editor &editor, event::EventDispatcher &dispatcher);

    void KeyboardListener::handle(const event::KeyboardEvent& e) {
        if (e.getKey() == CTRL_KEY('k')) {
            editor::MODE next = editor.getMode() == editor::WRITING
                ? editor::PHILOSOPHICAL
                : editor::WRITING;

            event::ModeEvent modeEvent(next);
            dispatcher.fire(modeEvent);

            return;
        }

        if (editor.getMode() == editor::PHILOSOPHICAL) {
            handlePhilosophicalMode(e, editor, dispatcher);
            return;
        }
    }

    static void handlePhilosophicalMode(const event::KeyboardEvent& e, editor::Editor &editor, event::EventDispatcher &dispatcher) {
        switch (e.getKey()) {
            case CTRL_KEY('z'):
                editor.undo();
                break;
            case CTRL_KEY('y'):
                editor.redo();
                break;
        }

        if (e.getKey() == CTRL_KEY('s')) {
            std::string target = tui::prompt("Search ", "Target: ");

            event::SearchEvent event(target, target.size());
            dispatcher.fire(event);

            return;
        }

        for (int i = 0; i < MOVE_CHARS_SIZE; i++) {
            if (e.getKey() == moveChars[i]) {
                event::MovementEvent moveEvent(e.getKey());
                dispatcher.fire(moveEvent);

                break;
            }
        }
    }

}
