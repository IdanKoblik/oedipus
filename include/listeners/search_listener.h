#ifndef SEARCH_LISTENER_H
#define SEARCH_LISTENER_H

#include "editor.h"
#include "listener.h"
#include "events/search_event.h"

namespace listener {
    class SearchListener : public IListener<event::SearchEvent> {
    public:
        editor::TextEditor& editor;

        explicit SearchListener(editor::TextEditor& e) : editor(e) {}

        void handle(const event::SearchEvent& e) override;
    };
}

#endif // SEARCH_LISTENER_H