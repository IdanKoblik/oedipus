#ifndef MOVE_LISTENER_H
#define MOVE_LISTENER_H

#include "keyboard_listener.h"
#include "events/move_event.h"

namespace listener {
    class MoveListener : public IListener<event::MovementEvent> {
    private:
        editor::Editor& editor;

    public:
        explicit MoveListener(editor::Editor& e) : editor(e) {}

        void handle(const event::MovementEvent& e) override;
    };
}

#endif // MOVE_LISTENER_H
