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
    public:
        static EventDispatcher& instance() {
            static EventDispatcher instance;
            return instance;
        }

        EventDispatcher() = default;
        ~EventDispatcher() = default;

        void registerListener(listener::IListenerBase *l);
        void clearListeners();

        void fire(IEvent& event) const;

        EventDispatcher(const EventDispatcher&) = delete;
        EventDispatcher& operator=(const EventDispatcher&) = delete;

        EventDispatcher(EventDispatcher&&) = delete;
        EventDispatcher& operator=(EventDispatcher&&) = delete;
    private:
        std::vector<listener::IListenerBase*> listeners;
    };
}

#endif // EVENT_H