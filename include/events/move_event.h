#ifndef MOVE_EVENT_H
#define MOVE_EVENT_H

#include "keyboard_event.h"

namespace event {

    class MovementEvent : public KeyboardEvent {
    public:
        explicit MovementEvent(char k) : KeyboardEvent(k) {}

        void dispatchTo(listener::IListenerBase* l) override  {
            if (auto* typed =dynamic_cast<listener::IListener<MovementEvent>*>(l))
                typed->handle(*this);
        }
    };

}

#endif // MOVE_EVENT_H
