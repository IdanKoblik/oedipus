#include "listeners/search_listener.h"

#include "kmp.h"

namespace listener {

    void SearchListener::handle(const event::SearchEvent &e) {
        if (editor.getMode() == editor::WRITING)
            return;

        editor::search_t &searchState = editor.getSearchState();
        auto lines = editor.getCake().getLines();
        for (size_t y = 0; y < lines.size(); ++y) {
            const std::string& line = lines[y];
            std::vector<size_t> res = kmp::search(line, e.getTarget());
            if (res.empty())
                continue;

            searchState.matches[y] = res;
            searchState.positions.push_back(editor::cursor_t{res[0], y});
            searchState.targetSize = e.getTargetSize();
            searchState.active = true;
        }
    }

}
