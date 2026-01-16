#include "net/client.h"

#include "proto/editor.pb.h"
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "global.h"

void Client::start(const NetworkBinding& binding) {
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        throw std::runtime_error("Cannot start client socket");

    sockaddr_in server{};
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(binding.port);
    if (inet_pton(AF_INET, binding.ip.c_str(), &server.sin_addr) != 1) {
        ::close(fd);
        throw std::runtime_error("Cannot parse given ip");
    }

    if (connect(fd, reinterpret_cast<sockaddr*>(&server), sizeof(server)) < 0) {
        ::close(fd);
        throw std::runtime_error("Cannot connect to to the server");
    }

    oedipus::ConnectReq req;
    req.set_clientid(123); // todo fix

    if (!sendProto(fd, req)) {
        ::close(fd);
        throw std::runtime_error("Cannot send protobuf req message to the server");
    }

    oedipus::ConnectAck ack;
    if (!recvProto(fd, ack) || !ack.ok()) {
        ::close(fd);
        throw std::runtime_error("Client was not able to receive / parse client ack");
    }
    
    this->id = 123;
    this->fd = fd;
    this->active = true;
}

void Client::close() {
    if (!this->active)
        return;

    ::close(this->fd);
    this->active = false;
}

std::string Client::downloadFile() {
    if (!this->active)
        return std::string{};

    const std::string filePath = "/tmp/oedipus_cwm_temp_file";
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) 
        return std::string{};

    while (true) {
        uint32_t len = 0;
        ssize_t n = read(fd, &len, sizeof(len));
        if (n == 0)
            break;

        if (n < 0)
            return std::string{};

        len = ntohl(len);
        std::vector<uint8_t> buffer(len);
        size_t rec = 0;
        while (rec < len) {
            ssize_t r = read(fd, buffer.data() + rec, len - rec);
            if (r <= 0) 
                return std::string{};

            rec += r;
        }

        oedipus::FileChunk chunk;
        if (!chunk.ParseFromArray(buffer.data(), buffer.size())) 
            return std::string{};

        outFile.write(chunk.data().data(), chunk.data().size());
        if (chunk.last()) 
            break;
    }

    outFile.close();
    return filePath;
}