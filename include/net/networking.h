#ifndef NETWORKING_H
#define NETWORKING_H

#include <string>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <google/protobuf/message.h>

struct NetworkBinding {
    std::string ip;
    uint16_t port;
    std::string addr;
};

bool recvProto(int fd, google::protobuf::Message& msg);

bool sendProto(int fd, const google::protobuf::Message& msg);


#endif // NETWORKING_H