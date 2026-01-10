#include "net/networking.h"

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int startSocket(const NetworkBinding& binding, const Role role) {
    if (binding.port <= 0 || binding.port > 65535)
        return -1;

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0)
        return -1;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(binding.port);

    if (inet_pton(AF_INET, binding.ip.c_str(), &addr.sin_addr) != 1) {
        close(socketFd);
        return -1;
    }

    if (role == SERVER) {
        const int opt = 1;
        setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (bind(socketFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            close(socketFd);
            return -1;
        }

        if (listen(socketFd, 128) < 0) {
            close(socketFd);
            return -1;
        }
    } else {
        if (connect(socketFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            close(socketFd);
            return -1;
        }
    }

    return socketFd;
}