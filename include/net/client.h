#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <google/protobuf/message.h>
#include "net/networking.h"
#include "proto/editor.pb.h"

class Client {
public:
    int fd = -1;
    uint64_t id;
    bool active = false;
    NetworkBinding binding;

    Client() = default;
    ~Client() = default;

    void start(const NetworkBinding& binding);
    void wait();
    void close();

    void startRecvThread();
    std::vector<oedipus::EditorOp> getPendingOps();

    void sendOp(const oedipus::EditorOp& op);

    std::string downloadFile();
private:
    void recvThreadLoop();

    std::mutex mutex;
    std::condition_variable cv;
    std::deque<oedipus::EditorOp> opQueue;
    std::mutex queueMutex;
    std::thread recvThread;
    bool recvThreadStarted = false;
};

#endif // CLIENT_H