#include "listeners/move_listener.h"

#include "events/move_event.h"
#include "listeners/mode_listener.h"

namespace listener {

    static size_t n = 0;

    void handleSearchMoving(editor::Editor &editor);

    void MoveListener::handle(const event::MovementEvent &e) {
        if (editor.getMode() == editor::WRITING)
            return;

        editor::cursor &cur = editor.getCursor();
        cake::Cake &cake = editor.getCake();
        editor::search &searchState = editor.getSearchState();
        
        if (e.getKey() == 'n' && searchState.active) {
            handleSearchMoving(editor);
            return;
        }

        switch (e.getKey()) {
            case 'h':
                if (cur.x > 1) {
                    cur.x--;
                } else if (cur.y > 0) {
                    cur.y--;
                    cur.x = cake.lineLength(cur.y) + 1;
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

    void handleSearchMoving(editor::Editor &editor) {
        std::vector<editor::cursor> positions = editor.getSearchState().positions;
        if (n > positions.size() - 1) {
            n = 0;
            return;
        }

        editor::cursor pos = positions[n];
        pos.x++;
        editor.getCursor() = pos;
        n++;
    }

}