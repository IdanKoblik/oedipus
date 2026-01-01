#ifndef MODE_EVENT_H
#define MODE_EVENT_H

#include "events/event.h"
#include "editor.h"

namespace event {

    class ModeEvent : public IEvent {
    private:
        editor::MODE mode;

    public:
        explicit ModeEvent(editor::MODE m) : mode(m) {}
        editor::MODE getMode() const { return mode; }

        void dispatchTo(listener::IListenerBase* l) override {
            if (auto* typed = dynamic_cast<listener::IListener<ModeEvent>*>(l))
                typed->handle(*this);
        }
    };

}

#endif // MODE_EVENT_H