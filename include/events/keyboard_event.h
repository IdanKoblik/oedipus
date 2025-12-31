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

        void dispatchTo(listener::IListenerBase* l) override;
    };

}

#endif // KEYBOARD_EVENT_H
