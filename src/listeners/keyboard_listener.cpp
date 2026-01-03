#include "listeners/keyboard_listener.h"

#include <cstring>

#include "kmp.h"
#include "terminal.h"
#include "events/move_event.h"
#include "events/search_event.h"
#include "tui/alert.h"
#include "tui/menu.h"

#define MOVE_CHARS_SIZE 5
constexpr char moveChars[MOVE_CHARS_SIZE] = { 'h', 'j', 'k', 'l', 'n' };

namespace listener {

    void KeyboardListener::handle(const event::KeyboardEvent& e) {
        if (e.getKey() == CTRL_KEY('k')) {
            editor::MODE next = editor.getMode() == editor::WRITING
                ? editor::PHILOSOPHICAL
                : editor::WRITING;

            event::ModeEvent modeEvent(next);
            dispatcher.fire(modeEvent);

            return;
        }

        for (int i = 0; i < MOVE_CHARS_SIZE; i++) {
            if (e.getKey() == moveChars[i]) {
                event::MovementEvent moveEvent(e.getKey());
                dispatcher.fire(moveEvent);

                break;
            }
        }

        if (e.getKey() == CTRL_KEY('s') && editor.getMode() == editor::PHILOSOPHICAL) {
            std::string target = tui::prompt("Search ", "Target: ");

            event::SearchEvent event(target, target.size());
            dispatcher.fire(event);

            return;
        }
    }

}
