#include "context.h"

Context::~Context() {
    this->stopAll();
}

void Context::startClient(const NetworkBinding& bind) {
    if (this->client)
        return;

    this->client = std::make_unique<Client>();
    this->clientThread = std::thread([this, bind] { client->start(bind); });
}

void Context::stopClient() {
    if (!this->client)
        return;

    this->client->close();
    if (this->clientThread.joinable())
        this->clientThread.join();
}

bool Context::hasClient() const {
    return this->client != nullptr;
}

Client& Context::clientRef() {
    if (!this->client)
        throw std::logic_error("Client not running");

    return *this->client;
}

void Context::startServer(const NetworkBinding& bind, const std::string& path) {
    if (this->server)
        return;

    this->server = std::make_unique<Server>();
    this->server->start(bind);
    this->serverThread = std::thread([this, path] { server->handle(path); });
}

void Context::stopServer() {
    if (!this->server)
        return;

    this->server->close();
    if (this->serverThread.joinable())
        this->serverThread.join();
}

bool Context::hasServer() const {
    return this->server != nullptr;
}

Server& Context::serverRef() {
    if (!this->server)
        throw std::logic_error("Server not running");

    return *this->server;
}

void Context::stopAll() {
    stopClient();
    stopServer();
}