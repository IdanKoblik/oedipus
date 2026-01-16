#ifndef KEYBOARD_LISTENER_H
#define KEYBOARD_LISTENER_H

#include "listeners/listener.h"
#include "events/keyboard_event.h"
#include "events/mode_event.h"
#include "editor.h"
#include "context.h"

namespace listener {

    class KeyboardListener : public IListener<event::KeyboardEvent> {
    public:
        Context& ctx;
        editor::TextEditor& editor;

        explicit KeyboardListener(editor::TextEditor& e, Context& ctx) : editor(e), ctx(ctx) {}

        void handle(const event::KeyboardEvent& e) override;
    private:
        static void handlePhilosophicalMode(const char key, editor::TextEditor& editor, Context& ctx);
        static void handleWritingMode(const char key, editor::TextEditor& editor, Context& ctx);
    };

}

#endif // KEYBOARD_LISTENER_H