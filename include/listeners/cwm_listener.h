#ifndef CWM_LISTENER_H
#define CWM_LISTENER_H

#include "editor.h"
#include "listener.h"
#include "events/cwm_event.h"

namespace listener {

    class CwmListener : public IListener<event::CwmEvent> {
    public:
        editor::TextEditor& editor;

        explicit CwmListener(editor::TextEditor& e) : editor(e) {}

        void handle(const event::CwmEvent& e) override;
    };
}

#endif // CWM_LISTENER_H