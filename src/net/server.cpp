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

void Server::handleClient(int clientFd, const std::string& path) {
    std::thread([this, clientFd, path]() {
        oedipus::ConnectReq req;
        if (!recvProto(clientFd, req)) {
            ::close(clientFd);
            return;
        }

        oedipus::ConnectAck ack;
        ack.set_ok(true);
        ack.set_message("Connected");
        sendProto(clientFd, ack);

        this->sendFile(path, req.clientid(), clientFd);
        while (this->active) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        ::close(clientFd);
    }).detach();
}

void Server::start(const NetworkBinding& binding) {
    if (this->active)
        return;

    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) 
        throw std::runtime_error("Cannot start server socket");

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(binding.port);
    inet_pton(AF_INET, binding.ip.c_str(), &addr.sin_addr);

    if (bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        ::close(fd);
        throw std::runtime_error("Cannot bind server socket");
    }

    if (listen(fd, 16) < 0) {
        ::close(fd);
        throw std::runtime_error("Cannot listen to server socket");
    }

    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    this->binding = binding;
    this->fd = fd;
    this->active = true;

//    state.serverThread = std::thread(acceptLogic, editor);
}

void Server::close() {
    this->active = false;
    shutdown(this->fd, SHUT_RDWR);
    ::close(this->fd);
}

void Server::handle(const std::string& path) {
    if (this->fd == -1)
        return;
    
    while (this->active) {
        sockaddr_in addr{};
        socklen_t len = sizeof(addr);

        int clientFd = accept(this->fd, (sockaddr*)&addr, &len);
        if (clientFd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            continue;
        }
    
        handleClient(clientFd, path);
    }
}

void Server::sendFile(const std::string& path, uint64_t clientId, int clientFd) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) 
        throw std::runtime_error("Cannot find target file");

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

        if (!sendProto(clientFd, chunk)) 
            throw std::runtime_error("Failed to send FileChunk");
    }
}