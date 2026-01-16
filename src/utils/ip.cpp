#include "utils/ip.h"

#include <regex>
#include <string>
#include "utils/logger.h"

namespace utils {

    NetworkBinding extractBinding(const std::string& input) {
        LOG_DEBUG("Extracting network binding from: " + input);
        size_t colonPos = input.find(':');
        if (colonPos == std::string::npos) {
            LOG_ERROR("Invalid network binding format (missing colon): " + input);
            throw std::invalid_argument("Input must be in format <ip>:<port>");
        }

        std::string ipPart = input.substr(0, colonPos);
        std::string portPart = input.substr(colonPos + 1);

        int port;
        try {
            port = std::stoi(portPart);
        } catch (...) {
            LOG_ERROR("Invalid port number: " + portPart);
            throw std::invalid_argument("Port must be a number: " + portPart);
        }

        if (port < 1 || port > 65535) {
            LOG_ERROR("Port out of range: " + std::to_string(port));
            throw std::out_of_range("Port must be between 1 and 65535: " + std::to_string(port));
        }

        LOG_DEBUG("Successfully extracted binding: " + ipPart + ":" + std::to_string(port));
        return NetworkBinding{
            ipPart,
            static_cast<uint16_t>(port),
            input
        };
    }
    
}