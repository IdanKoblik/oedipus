#include "listeners/keyboard_listener.h"

#include "terminal.h"
#include "events/move_event.h"

constexpr char moveChars[4] = { 'h', 'j', 'k', 'l' };

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

        for (int i = 0; i < 4; i++) {
            if (e.getKey() == moveChars[i]) {
                event::MovementEvent moveEvent(e.getKey());
                dispatcher.fire(moveEvent);

                break;
            }
        }
    }

}
