#ifndef SEARCH_LISTENER_H
#define SEARCH_LISTENER_H

#include "editor.h"
#include "listener.h"
#include "events/search_event.h"

namespace listener {
    class SearchListener : public IListener<event::SearchEvent> {
    private:
        editor::Editor& editor;

    public:
        explicit SearchListener(editor::Editor& e) : editor(e) {}

        void handle(const event::SearchEvent& e) override;
    };
}

#endif // SEARCH_LISTENER_H