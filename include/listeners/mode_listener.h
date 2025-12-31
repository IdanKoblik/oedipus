#ifndef MODE_LISTENER_H
#define MODE_LISTENER_H

#include "listeners/listener.h"
#include "events/mode_event.h"
#include "editor.h"

namespace listener {

    class ModeListener : public IListener<event::ModeEvent> {
    private:
        editor::Editor& editor;

    public:
        explicit ModeListener(editor::Editor& e) : editor(e) {}

        void handle(const event::ModeEvent& e) override;
    };

}

#endif // MODE_LISTENER_H