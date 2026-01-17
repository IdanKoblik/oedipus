#include "net/server.h"

#include "proto/editor.pb.h"
#include <vector>
#include <fstream>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <klogger/logger.h>

void Server::handleClient(int clientFd, const std::string& path) {
    LOG_INFO("New client connected, fd=" + std::to_string(clientFd));
    std::thread([this, clientFd, path]() {
        try {
            oedipus::ConnectReq req;
            if (!recvProto(clientFd, req)) {
                LOG_WARN("Failed to receive ConnectReq from client fd=" + std::to_string(clientFd));
                ::close(clientFd);
                return;
            }

            LOG_DEBUG("Received ConnectReq from client id=" + std::to_string(req.clientid()));
            oedipus::ConnectAck ack;
            ack.set_ok(true);
            ack.set_message("Connected");
            sendProto(clientFd, ack);

            this->sendFile(path, req.clientid(), clientFd);
            while (this->active) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            LOG_INFO("Closing client connection fd=" + std::to_string(clientFd));
            ::close(clientFd);
        } catch (const std::exception& e) {
            LOG_EXCEPTION(e);
            LOG_ERROR("Error in client handler thread: " + std::string(e.what()));
            ::close(clientFd);
        } catch (...) {
            LOG_UNKNOWN_EXCEPTION();
            LOG_ERROR("Unknown error in client handler thread");
            ::close(clientFd);
        }
    }).detach();
}

void Server::start(const NetworkBinding& binding) {
    if (this->active) {
        LOG_WARN("Attempted to start server but server is already active");
        return;
    }

    LOG_INFO("Starting server on " + binding.ip + ":" + std::to_string(binding.port));
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        LOG_ERROR("Failed to create server socket: errno=" + std::to_string(errno));
        throw std::runtime_error("Cannot start server socket");
    }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(binding.port);
    inet_pton(AF_INET, binding.ip.c_str(), &addr.sin_addr);

    if (bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        ::close(fd);
        LOG_ERROR("Failed to bind server socket: errno=" + std::to_string(errno));
        throw std::runtime_error("Cannot bind server socket");
    }

    if (listen(fd, 16) < 0) {
        ::close(fd);
        LOG_ERROR("Failed to listen on server socket: errno=" + std::to_string(errno));
        throw std::runtime_error("Cannot listen to server socket");
    }

    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    this->binding = binding;
    this->fd = fd;
    this->active = true;
    LOG_INFO("Server started successfully");
}

void Server::close() {
    if (!this->active)
        return;
        
    LOG_INFO("Closing server");
    this->active = false;
    shutdown(this->fd, SHUT_RDWR);
    ::close(this->fd);
}

void Server::handle(const std::string& path) {
    if (this->fd == -1) {
        LOG_WARN("Server handle called but fd is invalid");
        return;
    }
    
    LOG_INFO("Server accepting connections for path: " + path);
    while (this->active) {
        sockaddr_in addr{};
        socklen_t len = sizeof(addr);

        int clientFd = accept(this->fd, (sockaddr*)&addr, &len);
        if (clientFd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            LOG_WARN("Accept failed: errno=" + std::to_string(errno));
            continue;
        }
    
        handleClient(clientFd, path);
    }
    LOG_INFO("Server handle loop exited");
}

void Server::sendFile(const std::string& path, uint64_t clientId, int clientFd) {
    LOG_INFO("Sending file to client id=" + std::to_string(clientId) + " path=" + path);
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        LOG_ERROR("Cannot open file for sending: " + path);
        throw std::runtime_error("Cannot find target file");
    }

    const size_t CHUNK_SIZE = 4096;
    std::vector<char> buffer(CHUNK_SIZE);
    uint32_t seq = 0;

    while (file) {
        file.read(buffer.data(), buffer.size());
        std::streamsize n = file.gcount();
        if (n <= 0)
            break;

        oedipus::FileChunk chunk;
        chunk.set_clientid(clientId);
        chunk.set_seq(seq++);
        chunk.set_data(buffer.data(), n);
        chunk.set_last(file.eof());

        if (!sendProto(clientFd, chunk)) {
            LOG_ERROR("Failed to send FileChunk seq=" + std::to_string(seq-1) + " to client id=" + std::to_string(clientId));
            throw std::runtime_error("Failed to send FileChunk");
        }
    }
    
    LOG_INFO("File sent successfully: " + std::to_string(seq) + " chunks sent to client id=" + std::to_string(clientId));
}