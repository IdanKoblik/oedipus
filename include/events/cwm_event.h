#ifndef CWM_EVENT_H
#define CWM_EVENT_H

#include "event.h"
#include "net/networking.h"
#include "tui/menu.h"

namespace event {

    class CwmEvent : public IEvent {
    public:
        std::string addr;
        tui::CwmOptions options;

        explicit CwmEvent(const std::string& addr, tui::CwmOptions options) : addr(addr), options(options) {}

        void dispatchTo(listener::IListenerBase* l) override {
            if (auto* typed =dynamic_cast<listener::IListener<CwmEvent>*>(l))
                typed->handle(*this);
        }
    };
}

#endif // CWM_EVENT_H