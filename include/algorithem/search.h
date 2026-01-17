#ifndef SEARCH_H
#define SEARCH_H

#include <vector>
#include <string>
#include <map>
#include "cursor.h"

struct SearchState {
    bool active = false;
    std::string target;
    size_t targetSize;
    std::vector<Cursor> positions;
    std::map<size_t, std::vector<size_t>> matches;
};

namespace search {

static std::vector<int> computeLPS(const std::string& pattern);

std::vector<size_t> kmp(const std::string& text, const std::string& pattern);

}

#endif // SEARCH_H