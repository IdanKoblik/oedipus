#include "net/client.h"

#include "proto/editor.pb.h"
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>

bool recvProto(int fd, google::protobuf::Message& msg) {
    uint32_t netLen = 0;
    ssize_t n = read(fd, &netLen, sizeof(netLen));
    if (n != sizeof(netLen))
        return false;
    uint32_t len = ntohl(netLen);
    std::vector<uint8_t> buffer(len);
    size_t rec = 0;
    while (rec < len) {
        ssize_t r = read(fd, buffer.data() + rec, len - rec);
        if (r <= 0)
            return false;
        rec += r;
    }
    
    return msg.ParseFromArray(buffer.data(), buffer.size());
}

std::string downloadFile(const int fd) {
    if (fd < 0)
        return std::string{};

    const std::string filePath = "/tmp/oedipus_cwm_temp_file";
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open " << filePath << std::endl;
        return std::string{};
    }

    while (true) {
        uint32_t len = 0;
        ssize_t n = read(fd, &len, sizeof(len));
        if (n == 0)
            break;

        if (n < 0) {
            perror("read");
            return std::string{};
        }

        len = ntohl(len);
        std::vector<uint8_t> buffer(len);
        size_t rec = 0;
        while (rec < len) {
            ssize_t r = read(fd, buffer.data() + rec, len - rec);
            if (r <= 0) {
                perror("read");
                return "";
            }

            rec += r;
        }

        oedipus::FileChunk chunk;
        if (!chunk.ParseFromArray(buffer.data(), buffer.size())) {
            std::cerr << "Failed to parse FileChunk" << std::endl;
            return "";
        }

        outFile.write(chunk.data().data(), chunk.data().size());
        if (chunk.last()) 
            break;
    }

    outFile.close();
    return filePath;
}