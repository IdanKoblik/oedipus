#ifndef MOVEMENT_EVENT_H
#define MOVEMENT_EVENT_H

#include "event.h"
#include "keyboard_event.h"

namespace event {
    class MovementEvent final : public KeyboardEvent {
    public:
        explicit MovementEvent(const char k) : KeyboardEvent(k) {}

        void dispatchTo(listener::IListenerBase* l) override  {
            if (auto* typed =dynamic_cast<listener::IListener<MovementEvent>*>(l))
                typed->handle(*this);
        }
    };
}

#endif // MOVEMENT_EVENT_H
