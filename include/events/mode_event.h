#ifndef MODE_EVENT_H
#define MODE_EVENT_H

#include "editor.h"
#include "events/event.h"

namespace event {

    class ModeEvent : public IEvent {
    public:
        editor::EditorMode mode;

        explicit ModeEvent(const editor::EditorMode m) : mode(m) {}

        void dispatchTo(listener::IListenerBase* l) override {
            if (auto* typed = dynamic_cast<listener::IListener<ModeEvent>*>(l))
                typed->handle(*this);
        }
    };
}

#endif // MODE_EVENT_H