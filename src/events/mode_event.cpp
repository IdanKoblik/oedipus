#include "events/mode_event.h"
#include "listeners/listener.h"

namespace event {

    void ModeEvent::dispatchTo(listener::IListenerBase* l) {
        if (auto* typed = dynamic_cast<listener::IListener<ModeEvent>*>(l))
            typed->handle(*this);
    }

}
