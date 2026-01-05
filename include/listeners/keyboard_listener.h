#ifndef KEYBOARD_LISTENER_H
#define KEYBOARD_LISTENER_H

#include "listeners/listener.h"
#include "events/keyboard_event.h"
#include "events/mode_event.h"
#include "editor.h"

namespace listener {

    class KeyboardListener : public IListener<event::KeyboardEvent> {
    public:
        editor::TextEditor& editor;

        explicit KeyboardListener(editor::TextEditor& e) : editor(e) {}

        void handle(const event::KeyboardEvent& e) override;
    private:
        static void handlePhilosophicalMode(const char key, editor::TextEditor& editor);
        static void handleWritingMode(const char key, editor::TextEditor& editor);
    };

}

#endif // KEYBOARD_LISTENER_H