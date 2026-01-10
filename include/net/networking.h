#ifndef NETWORKING_H
#define NETWORKING_H

#include <string>

struct NetworkBinding {
    std::string ip;
    int port;
    std::string addr;
};

enum Role {
    CLIENT,
    SERVER
};

struct NetworkingState {
    NetworkBinding binding;
    int socket;
    Role role;
    bool active = false;
};

int startSocket(const NetworkBinding& binding, const Role role);

#endif // NETWORKING_H