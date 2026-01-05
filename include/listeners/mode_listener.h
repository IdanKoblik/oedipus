#ifndef MODE_LISTENER_H
#define MODE_LISTENER_H

#include "listeners/listener.h"
#include "events/mode_event.h"
#include "editor.h"

namespace listener {

    class ModeListener : public IListener<event::ModeEvent> {
    public:
        editor::TextEditor& editor;

        explicit ModeListener(editor::TextEditor& e) : editor(e) {}

        void handle(const event::ModeEvent& e) override;
    };

}

#endif // MODE_LISTENER_H