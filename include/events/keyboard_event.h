#ifndef KEYBOARD_EVENT_H
#define KEYBOARD_EVENT_H
#include "event.h"

namespace event {

    class KeyboardEvent : public IEvent {
    public:
        char key;

        explicit KeyboardEvent(const char k) : key(k) {}

        void dispatchTo(listener::IListenerBase* l) override {
            if (auto* typed =dynamic_cast<listener::IListener<KeyboardEvent>*>(l))
                typed->handle(*this);
        }
    };
}

#endif // KEYBOARD_EVENT_H
