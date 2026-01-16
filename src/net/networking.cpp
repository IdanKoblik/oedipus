#include "net/networking.h"

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

bool sendProto(int fd, const google::protobuf::Message& msg) {
    uint32_t size = msg.ByteSizeLong();
    uint32_t netSize = htonl(size);

    if (write(fd, &netSize, sizeof(netSize)) != sizeof(netSize))
        return false;

    std::vector<uint8_t> buffer(size);
    msg.SerializeToArray(buffer.data(), buffer.size());

    size_t sent = 0;
    while (sent < buffer.size()) {
        ssize_t n = write(fd, buffer.data() + sent, buffer.size() - sent);
        if (n <= 0)
            return false;
        sent += n;
    }
    return true;
}