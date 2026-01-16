#include "net/client.h"

#include "proto/editor.pb.h"
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include "global.h"
#include "utils/logger.h"

void Client::start(const NetworkBinding& binding) {
    LOG_INFO("Starting client connection to " + binding.ip + ":" + std::to_string(binding.port));
    
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        LOG_ERROR("Failed to create client socket: errno=" + std::to_string(errno));
        throw std::runtime_error("Cannot start client socket");
    }

    sockaddr_in server{};
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(binding.port);
    if (inet_pton(AF_INET, binding.ip.c_str(), &server.sin_addr) != 1) {
        ::close(fd);
        LOG_ERROR("Failed to parse IP address: " + binding.ip);
        throw std::runtime_error("Cannot parse given ip");
    }

    if (connect(fd, reinterpret_cast<sockaddr*>(&server), sizeof(server)) < 0) {
        ::close(fd);
        LOG_ERROR("Failed to connect to server " + binding.ip + ":" + std::to_string(binding.port) + " errno=" + std::to_string(errno));
        throw std::runtime_error("Cannot connect to to the server");
    }

    oedipus::ConnectReq req;
    req.set_clientid(123); // todo fix

    if (!sendProto(fd, req)) {
        ::close(fd);
        LOG_ERROR("Failed to send ConnectReq protobuf message");
        throw std::runtime_error("Cannot send protobuf req message to the server");
    }

    oedipus::ConnectAck ack;
    if (!recvProto(fd, ack) || !ack.ok()) {
        ::close(fd);
        LOG_ERROR("Failed to receive or parse ConnectAck, or ack.ok() is false");
        throw std::runtime_error("Client was not able to receive / parse client ack");
    }
    
    this->id = 123;
    this->fd = fd;
    this->active = true;
    LOG_INFO("Client connection established successfully");
}

void Client::close() {
    if (!this->active)
        return;

    LOG_INFO("Closing client connection");
    ::close(this->fd);
    this->active = false;
}

std::string Client::downloadFile() {
    if (!this->active) {
        LOG_WARN("Attempted to download file but client is not active");
        return std::string{};
    }

    LOG_INFO("Starting file download");
    const std::string filePath = "/tmp/oedipus_cwm_temp_file";
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {
        LOG_ERROR("Failed to open temporary file for download: " + filePath);
        return std::string{};
    }

    uint32_t chunkCount = 0;
    while (true) {
        uint32_t len = 0;
        ssize_t n = read(fd, &len, sizeof(len));
        if (n == 0) {
            LOG_DEBUG("Received EOF during file download");
            break;
        }

        if (n < 0) {
            LOG_ERROR("Read error during file download: errno=" + std::to_string(errno));
            outFile.close();
            return std::string{};
        }

        len = ntohl(len);
        std::vector<uint8_t> buffer(len);
        size_t rec = 0;
        while (rec < len) {
            ssize_t r = read(fd, buffer.data() + rec, len - rec);
            if (r <= 0) {
                LOG_ERROR("Read error while receiving chunk data: errno=" + std::to_string(errno));
                outFile.close();
                return std::string{};
            }

            rec += r;
        }

        oedipus::FileChunk chunk;
        if (!chunk.ParseFromArray(buffer.data(), buffer.size())) {
            LOG_ERROR("Failed to parse FileChunk protobuf message");
            outFile.close();
            return std::string{};
        }

        outFile.write(chunk.data().data(), chunk.data().size());
        chunkCount++;
        if (chunk.last()) {
            LOG_INFO("File download completed: " + std::to_string(chunkCount) + " chunks received");
            break;
        }
    }

    outFile.close();
    LOG_INFO("File downloaded successfully to: " + filePath);
    return filePath;
}