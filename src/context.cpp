#include "context.h"

#include <klogger/logger.h>

Context::~Context() {
    this->stopAll();
}

void Context::startClient(const NetworkBinding& bind) {
    if (this->client) {
        LOG_WARN("Attempted to start client but client is already running");
        return;
    }

    LOG_INFO("Starting client thread");
    this->client = std::make_unique<Client>();
    this->clientThread = std::thread([this, bind] { 
        try {
            client->start(bind);
        } catch (const std::exception& e) {
            LOG_EXCEPTION(e);
            LOG_ERROR("Client thread crashed: " + std::string(e.what()));
        } catch (...) {
            LOG_UNKNOWN_EXCEPTION();
            LOG_ERROR("Client thread crashed with unknown exception");
        }
    });
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
    if (!this->client) {
        LOG_ERROR("Attempted to access client reference but client is not running");
        throw std::logic_error("Client not running");
    }

    return *this->client;
}

void Context::startServer(const NetworkBinding& bind, const std::string& path) {
    if (this->server) {
        LOG_WARN("Attempted to start server but server is already running");
        return;
    }

    LOG_INFO("Starting server thread for path: " + path);
    this->server = std::make_unique<Server>();
    try {
        this->server->start(bind);
    } catch (const std::exception& e) {
        LOG_EXCEPTION(e);
        LOG_ERROR("Failed to start server: " + std::string(e.what()));
        throw;
    }
    this->serverThread = std::thread([this, path] { 
        try {
            server->handle(path);
        } catch (const std::exception& e) {
            LOG_EXCEPTION(e);
            LOG_ERROR("Server thread crashed: " + std::string(e.what()));
        } catch (...) {
            LOG_UNKNOWN_EXCEPTION();
            LOG_ERROR("Server thread crashed with unknown exception");
        }
    });
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
    if (!this->server) {
        LOG_ERROR("Attempted to access server reference but server is not running");
        throw std::logic_error("Server not running");
    }

    return *this->server;
}

void Context::stopAll() {
    stopClient();
    stopServer();
}