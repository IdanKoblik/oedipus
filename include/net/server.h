#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <google/protobuf/message.h>
#include "net/networking.h"
#include "proto/editor.pb.h"
#include "editor.h"

class Server {
public:
    int fd = -1;
    bool active = false;
    
    uint64_t clientId;
    int clientFd = -1;

    NetworkBinding binding;

    Server() = default;
    ~Server() = default;

    void start(const NetworkBinding& bind);
    void wait();
    void close();

    void broadcastOp(const oedipus::EditorOp& op);

    void handle(const std::string& path, editor::TextEditor* editor);
private:
    std::mutex mutex;
    std::condition_variable cv;

    void sendFile(const std::string& path, uint64_t clientId, int clientFd);

    void handleClient(int clientFd, const std::string& path, editor::TextEditor* editor);
};

#endif // SERVER_H