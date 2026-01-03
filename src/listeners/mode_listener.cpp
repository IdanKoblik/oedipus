#include "listeners/mode_listener.h"

namespace listener {

    void ModeListener::handle(const event::ModeEvent& e) {
        editor.setMode(e.getMode());
        editor.getSearchState() = {};

        if (e.getMode() == editor::PHILOSOPHICAL)
            editor.getCake().saveToFile(editor.getPath());
    }

}