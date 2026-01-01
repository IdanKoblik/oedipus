#ifndef KEYBOARD_EVENT_H
#define KEYBOARD_EVENT_H

#include "events/event.h"

namespace event {

    class KeyboardEvent : public IEvent {
    protected:
        char key;

    public:
        explicit KeyboardEvent(char k) : key(k) {}
        char getKey() const { return key; }

        void dispatchTo(listener::IListenerBase* l) override {
            if (auto* typed =dynamic_cast<listener::IListener<KeyboardEvent>*>(l))
                typed->handle(*this);
        }
    };

}

#endif // KEYBOARD_EVENT_H
