#ifndef NETWORKING_H
#define NETWORKING_H

#include <string>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include "client.h"

struct NetworkBinding {
    std::string ip;
    uint16_t port;
    std::string addr;
};

enum Role {
    CLIENT,
    SERVER,
    EXIT
};

struct NetworkingState {
    Role role;
    int fd = -1;
    bool active = false;

    std::thread serverThread;
    ClientConn client;
};

#endif // NETWORKING_H