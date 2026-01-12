#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <google/protobuf/message.h>

struct ClientConn {
    int fd = -1;
    uint64_t id;
};

bool recvProto(int fd, google::protobuf::Message& msg);

std::string downloadFile(const int fd);

#endif // CLIENT_H
