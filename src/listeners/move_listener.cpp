#include "listeners/move_listener.h"

#include "events/move_event.h"
#include "listeners/mode_listener.h"

namespace listener {

    void MoveListener::handle(const event::MovementEvent &e) {
        if (editor.getMode() == editor::WRITING)
            return;

        editor::cursor &cur = editor.getCursor();
        cake::Cake &cake = editor.getCake();

        switch (e.getKey()) {
            case 'h':
                if (cur.x > 1) {
                    cur.x--;
                } else if (cur.y > 0) {
                    cur.y--;
                    cur.x = cake.lineLength(cur.y);
                }
                break;

            case 'l': {
                size_t len = cake.lineLength(cur.y) + 1;
                if (cur.x < static_cast<int>(len)) {
                    cur.x++;
                } else if (cur.y + 1 < static_cast<int>(cake.lineCount())) {
                    cur.y++;
                    cur.x = 1;
                }
                break;
            }

            case 'k':
                if (cur.y > 0) {
                    cur.y--;
                }
                break;

            case 'j':
                if (cur.y + 1 < static_cast<int>(cake.lineCount()))
                    cur.y++;
                break;
        }

        size_t len = cake.lineLength(cur.y);
        if (cur.x > static_cast<int>(len) + 1)
            cur.x = len + 1;
    }

}