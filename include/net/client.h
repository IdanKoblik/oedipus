#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <mutex>
#include <condition_variable>
#include <google/protobuf/message.h>
#include "net/networking.h"
#include "proto/editor.pb.h"

class Client {
public:
    int fd = -1;
    uint64_t id;
    bool active = false;

    Client() = default;
    ~Client() = default;

    void start(const NetworkBinding& binding);
    void wait();
    void close();
    
    void sendOp(const oedipus::EditorOp& op);
    void recvOp();

    std::string downloadFile();
private:
    std::mutex mutex;
    std::condition_variable cv;
};

#endif // CLIENT_H