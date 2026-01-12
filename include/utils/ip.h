#ifndef IP_H
#define IP_H

#include <string>
#include "net/networking.h"

namespace utils {
    NetworkBinding extractBinding(const std::string& input);
}

#endif // IP_H
