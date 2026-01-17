#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <cstdint>
#include <mutex>
#include <condition_variable>
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
    void wait();
    void close();

    void handle(const std::string& path);
private:
    std::mutex mutex;
    std::condition_variable cv;

    void sendFile(const std::string& path, uint64_t clientId, int clientFd);

    void handleClient(int clientFd, const std::string& path);
};

#endif // SERVER_H