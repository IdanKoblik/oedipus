#include "listeners/mode_listener.h"

namespace listener {

    void ModeListener::handle(const event::ModeEvent &e) {
        this->editor.state.mode = e.mode;

        if (e.mode == editor::PHILOSOPHICAL)
            this->editor.cake.saveToFile(this->editor.path);
    }

}