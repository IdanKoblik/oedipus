#ifndef MOVE_LISTENER_H
#define MOVE_LISTENER_H

#include "keyboard_listener.h"
#include "events/movement_event.h"

namespace listener {
    class MovementListener : public IListener<event::MovementEvent> {
    public:
        editor::TextEditor& editor;

        explicit MovementListener(editor::TextEditor& e) : editor(e) {}

        void handle(const event::MovementEvent& e) override;
    };
}

#endif // MOVE_LISTENER_H