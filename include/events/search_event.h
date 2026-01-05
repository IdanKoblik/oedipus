#ifndef SEARCH_EVENT_H
#define SEARCH_EVENT_H

#include "event.h"
#include <string>

namespace event {

    class SearchEvent : public IEvent {
    public:
        const std::string target;
        const size_t targetSize;

        explicit SearchEvent(const std::string &target, const size_t targetSize) : target(target), targetSize(targetSize) {}

        void dispatchTo(listener::IListenerBase* l) override {
            if (auto* typed =dynamic_cast<listener::IListener<SearchEvent>*>(l))
                typed->handle(*this);
        }
    };
}

#endif // SEARCH_EVENT_H
