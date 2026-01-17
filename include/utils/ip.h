#ifndef IP_H
#define IP_H

#include <string>
#include "net/networking.h"

#define MAX_PORT 65535
#define MIN_PORT 1

namespace utils {

NetworkBinding extractBinding(const std::string& input);

}

#endif // IP_H