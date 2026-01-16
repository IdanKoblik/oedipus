#ifndef CONTEXT_H
#define CONTEXT_H

#include <memory>
#include <thread>
#include <string>
#include "net/client.h"
#include "net/server.h"

class Context {
public:
    Context() = default;
    ~Context();

    void startClient(const NetworkBinding& bind);
    void stopClient();

    bool hasClient() const;
    Client& clientRef();

    void startServer(const NetworkBinding& bind, const std::string& path);
    void stopServer();

    bool hasServer() const;
    Server& serverRef();

    void stopAll();

private:
    std::unique_ptr<Client> client;
    std::unique_ptr<Server> server;

    std::thread clientThread;
    std::thread serverThread;
};

#endif // CONTEXT_H