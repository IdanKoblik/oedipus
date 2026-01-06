#include "events/event.h"

namespace event {

    void EventDispatcher::registerListener(listener::IListenerBase* l) {
        listeners.push_back(l);
    }

    void EventDispatcher::clearListeners() {
        listeners.clear();
    }

    void EventDispatcher::fire(IEvent& event) const {
        for (auto* l : listeners)
            event.dispatchTo(l);
    }

}