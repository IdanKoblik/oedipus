#include "listeners/search_listener.h"

#include <cstring>
#include "search.h"

namespace listener {

    void SearchListener::handle(const event::SearchEvent &e) {
        SearchState& searchState = editor.state.search;
        const std::vector<std::string> lines = editor.cake.getLines();

        for (size_t y = 0; y < lines.size(); ++y) {
            const std::string& line = lines[y];
            std::vector<size_t> res = search::kmp(line, e.target);
            if (res.empty())
                continue;

            searchState.active = true;
            searchState.target = e.target;
            searchState.matches[y] = res;
            searchState.positions.push_back(Cursor{res[0], y});
            searchState.targetSize = strlen(e.target.c_str());
        }
    }

}
