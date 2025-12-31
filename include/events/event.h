#ifndef EVENT_H
#define EVENT_H

#include <vector>
#include "listeners/listener.h"

namespace event {

    class IEvent {
    public:
        virtual ~IEvent() = default;
        virtual void dispatchTo(listener::IListenerBase* listener) = 0;
    };

    class EventDispatcher {
        std::vector<listener::IListenerBase*> listeners;

    public:
        void registerListener(listener::IListenerBase* l);
        void fire(IEvent& event);
    };

}

#endif