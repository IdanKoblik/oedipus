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
        if (e.getKey() == editor.getConfig().keybindings[config::SWITCH_MOVE].shortcut) {
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
        if (e.getKey() == editor.getConfig().keybindings[config::UNDO].shortcut) {
            editor.undo();
            return;
        }

        if (e.getKey() == editor.getConfig().keybindings[config::REDO].shortcut) {
            editor.redo();
            return;
        }

        if (e.getKey() == editor.getConfig().keybindings[config::SEARCH_MOVE].shortcut) {
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
