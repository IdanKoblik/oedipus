#include "net/client.h"

#include "proto/editor.pb.h"
#include "net/networking.h"
#include "global.h"
#include <klogger/logger.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

void Client::start(const NetworkBinding& binding) {
    LOG_INFO("Starting client connection to " + binding.ip + ":" + std::to_string(binding.port));
    
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        LOG_ERROR("Failed to create client socket: errno=" + std::to_string(errno));
        throw std::runtime_error("Cannot start client socket");
    }

    sockaddr_in server{};
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(binding.port);
    if (inet_pton(AF_INET, binding.ip.c_str(), &server.sin_addr) != 1) {
        ::close(fd);
        LOG_ERROR("Failed to parse IP address: " + binding.ip);
        throw std::runtime_error("Cannot parse given ip");
    }

    if (connect(fd, reinterpret_cast<sockaddr*>(&server), sizeof(server)) < 0) {
        ::close(fd);
        LOG_ERROR("Failed to connect to server " + binding.ip + ":" + std::to_string(binding.port) + " errno=" + std::to_string(errno));
        throw std::runtime_error("Cannot connect to to the server");
    }

    oedipus::ConnectReq req;
    req.set_clientid(123); // todo fix

    if (!sendProto(fd, req)) {
        ::close(fd);
        LOG_ERROR("Failed to send ConnectReq protobuf message");
        throw std::runtime_error("Cannot send protobuf req message to the server");
    }

    oedipus::ConnectAck ack;
    if (!recvProto(fd, ack) || !ack.ok()) {
        ::close(fd);
        LOG_ERROR("Failed to receive or parse ConnectAck, or ack.ok() is false");
        throw std::runtime_error("Client was not able to receive / parse client ack");
    }
    
    {
        std::lock_guard lock(this->mutex);
        this->id = 123;
        this->fd = fd;
        this->binding = binding;
        this->active = true;
        LOG_INFO("Client connection established successfully");
    }
    this->cv.notify_all();
}

void Client::wait() {
    std::unique_lock lock(this->mutex);
    this->cv.wait(lock, [&] { return this->active; });
}

void Client::recvThreadLoop() {
    while (this->active) {
        oedipus::EditorOp op;
        if (!recvProto(this->fd, op)) {
            LOG_INFO("Recv thread: server disconnected or stream corrupted");
            break;
        }
        std::lock_guard lock(this->queueMutex);
        this->opQueue.push_back(std::move(op));
    }
}

void Client::startRecvThread() {
    std::lock_guard lock(this->mutex);
    if (this->recvThreadStarted) {
        LOG_WARN("Recv thread already started");
        return;
    }
    this->recvThreadStarted = true;
    this->recvThread = std::thread([this] { this->recvThreadLoop(); });
    LOG_INFO("Started recv thread for incoming editor ops");
}

std::vector<oedipus::EditorOp> Client::getPendingOps() {
    std::lock_guard lock(this->queueMutex);
    std::vector<oedipus::EditorOp> out;
    out.reserve(this->opQueue.size());
    while (!this->opQueue.empty()) {
        out.push_back(std::move(this->opQueue.front()));
        this->opQueue.pop_front();
    }
    return out;
}

void Client::close() {
    if (!this->active)
        return;

    LOG_INFO("Closing client connection");
    this->active = false;
    if (this->fd >= 0) {
        ::close(this->fd);
        this->fd = -1;
    }
    if (this->recvThreadStarted && this->recvThread.joinable()) {
        this->recvThread.join();
        this->recvThreadStarted = false;
    }
}

void Client::sendOp(const oedipus::EditorOp& op) {
    if (!this->active) {
        LOG_WARN("Attempted to send op but client is not active");
        return;
    }

    if (!sendProto(this->fd, op)) {
        LOG_ERROR("Failed to send EditorOp protobuf message to server");
    } else {
        LOG_DEBUG("Sent EditorOp protobuf message to server");
    }
}

std::string Client::downloadFile() {
    if (!this->active) {
        LOG_WARN("Attempted to download file but client is not active");
        return {};
    }

    LOG_INFO("Starting file download");
    const std::string filePath = "/tmp/oedipus_cwm_temp_file";
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {
        LOG_ERROR("Failed to open temporary file for download: " + filePath);
        return {};
    }

    uint32_t chunkCount = 0;

    while (true) {
        oedipus::FileChunk chunk;

        if (!recvProto(this->fd, chunk)) {
            LOG_ERROR("recvProto(FileChunk) failed or client disconnected");
            outFile.close();
            return {};
        }

        outFile.write(chunk.data().data(), chunk.data().size());
        chunkCount++;

        if (chunk.last()) {
            LOG_INFO("File download completed: " + std::to_string(chunkCount) + " chunks received");
            break;
        }
    }

    outFile.close();
    LOG_INFO("File downloaded successfully to: " + filePath);
    return filePath;
}
