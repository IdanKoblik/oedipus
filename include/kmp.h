#ifndef KMP_H
#define KMP_H

#include <string>
#include <vector>

namespace kmp {

    std::vector<size_t> search(const std::string &text, const std::string &target);

}

#endif // KMP_H