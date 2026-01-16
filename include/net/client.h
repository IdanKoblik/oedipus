#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <google/protobuf/message.h>
#include "net/networking.h"

class Client {
public:
    int fd = -1;
    uint64_t id;
    bool active = false;

    Client() = default;
    ~Client() = default;

    void start(const NetworkBinding& binding);
    void close();

    std::string downloadFile();
};

#endif // CLIENT_H
