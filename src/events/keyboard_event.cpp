#include "events/keyboard_event.h"
#include "listeners/listener.h"

namespace event {

    void KeyboardEvent::dispatchTo(listener::IListenerBase* l) {
        if (auto* typed =dynamic_cast<listener::IListener<KeyboardEvent>*>(l))
            typed->handle(*this);
    }

}
