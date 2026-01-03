#include "kmp.h"

#include <iostream>
#include <vector>

#include "tui/alert.h"

namespace kmp {

    std::vector<int> computeLPS(const std::string& pattern) {
        std::vector<int> lps(pattern.size(), 0);

        int len = 0;
        int i = 1;

        while (i < static_cast<int>(pattern.size())) {
            if (pattern[i] == pattern[len]) {
                len++;
                lps[i] = len;
                i++;
            } else {
                if (len != 0) {
                    len = lps[len - 1];
                } else {
                    lps[i] = 0;
                    i++;
                }
            }
        }

        return lps;
    }

    std::vector<size_t> search(
        const std::string& text,
        const std::string& pattern
    ) {
        std::vector<size_t> matches;

        if (pattern.empty() || text.empty())
            return matches;

        auto lps = computeLPS(pattern);

        size_t i = 0; // index for text
        size_t j = 0; // index for pattern

        while (i < text.size()) {
            if (text[i] == pattern[j]) {
                i++;
                j++;
            }

            if (j == pattern.size()) {
                matches.push_back(i - j);
                j = lps[j - 1];
            }
            else if (i < text.size() && text[i] != pattern[j]) {
                if (j != 0)
                    j = lps[j - 1];
                else
                    i++;
            }
        }

        return matches;
    }

}
