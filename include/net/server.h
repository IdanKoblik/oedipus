#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <cstdint>
#include <google/protobuf/message.h>
#include "net/networking.h"

class Server {
public:
    int fd = -1;
    bool active = false;
    uint64_t clientId;
    NetworkBinding binding;

    Server() = default;
    ~Server() = default;

    void start(const NetworkBinding& bind);
    void close();

    void handle(const std::string& path);
private:
    void sendFile(const std::string& path, uint64_t clientId, int clientFd);

    void handleClient(int clientFd, const std::string& path);
};

#endif // SERVER_H