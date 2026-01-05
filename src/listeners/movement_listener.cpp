#include "listeners/movement_listener.h"

namespace listener {

    void MovementListener::handle(const event::MovementEvent &e) {
        Cursor& cursor = this->editor.state.cursor;
        const cake::Cake& cake = this->editor.cake;

        switch (e.key) {
            case 'h': {
                if (cursor.x > 1) {
                    cursor.x--;
                } else if (cursor.y > 0) {
                    cursor.y--;
                    cursor.x = cake.lineLength(cursor.y) + 1;
                }
                break;
            }

            case 'l': {
                size_t len = cake.lineLength(cursor.y) + 1;
                if (cursor.x < static_cast<int>(len)) {
                    cursor.x++;
                } else if (cursor.y + 1 < static_cast<int>(cake.lineCount())) {
                    cursor.y++;
                    cursor.x = 1;
                }
                break;
            }

            case 'k': {
                if (cursor.y > 0) {
                    cursor.y--;
                }
                break;
            }

            case 'j': {
                if (cursor.y + 1 < static_cast<int>(cake.lineCount()))
                    cursor.y++;
                break;
            }
        }

        const size_t len = cake.lineLength(cursor.y);
        if (cursor.x > static_cast<int>(len) + 1)
            cursor.x = len + 1;
    }

}