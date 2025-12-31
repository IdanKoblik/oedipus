#ifndef KEYBOARD_LISTENER_H
#define KEYBOARD_LISTENER_H

#include "listeners/listener.h"
#include "events/keyboard_event.h"
#include "events/mode_event.h"
#include "editor.h"

namespace listener {

    class KeyboardListener : public IListener<event::KeyboardEvent> {
    private:
        editor::Editor& editor;
        event::EventDispatcher& dispatcher;

    public:
        KeyboardListener(editor::Editor& e, event::EventDispatcher& d)
            : editor(e), dispatcher(d) {}

        void handle(const event::KeyboardEvent& e) override;
    };

}

#endif // KEYBOARD_LISTENER_H