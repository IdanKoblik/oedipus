#ifndef SEARCH_EVENT_H
#define SEARCH_EVENT_H

#include "event.h"
#include <string>

namespace event {

    class SearchEvent : public IEvent {
    protected:
        const std::string target;
        const size_t targetSize;

    public:
        explicit SearchEvent(const std::string &target, const size_t targetSize) : target(target), targetSize(targetSize) {}
        std::string getTarget() const { return target; }
        size_t getTargetSize() const { return targetSize; }

        void dispatchTo(listener::IListenerBase* l) override {
            if (auto* typed =dynamic_cast<listener::IListener<SearchEvent>*>(l))
                typed->handle(*this);
        }
    };
}

#endif // SEARCH_EVENT_H