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

bool sendProto(int fd, const google::protobuf::Message& msg) {
    uint32_t size = msg.ByteSizeLong();
    uint32_t netSize = htonl(size);

    if (write(fd, &netSize, sizeof(netSize)) != sizeof(netSize))
        return false;

    std::vector<uint8_t> buffer(size);
    msg.SerializeToArray(buffer.data(), buffer.size());

    size_t sent = 0;
    while (sent < buffer.size()) {
        ssize_t n = write(fd, buffer.data() + sent, buffer.size() - sent);
        if (n <= 0)
            return false;
        sent += n;
    }
    return true;
}

bool sendFile(int fd, const std::string& path, uint64_t clientId) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return false;
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

        if (!sendProto(fd, chunk)) {
            std::cerr << "Failed to send FileChunk" << std::endl;
            return false;
        }
    }

    return true;
}

static void handleClient(editor::TextEditor* editor, int fd) {
    std::thread([editor, fd]() {
        oedipus::ConnectReq req;
        if (!recvProto(fd, req)) {
            close(fd);
            return;
        }

        editor->networking.client = ClientConn{ fd, req.clientid() };

        oedipus::ConnectAck ack;
        ack.set_ok(true);
        ack.set_message("Connected");
        sendProto(fd, ack);

        sendFile(fd, editor->path, req.clientid());

        while (editor->networking.active) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        close(fd);
    }).detach();
}

static void acceptLogic(editor::TextEditor* editor) {
    int serverFd = editor->networking.fd;

    while (editor->networking.active) {
        sockaddr_in addr{};
        socklen_t len = sizeof(addr);

        int clientFd = accept(serverFd, (sockaddr*)&addr, &len);

        if (clientFd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }
            perror("accept");
            continue;
        }

        if (editor->networking.client.fd != -1) {
            close(clientFd);
            continue;
        }

        handleClient(editor, clientFd);
    }
}


void startServer(editor::TextEditor* editor, const NetworkBinding& binding) {
    NetworkingState& state = editor->networking;
    if (state.active)
        return;

    state.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (state.fd < 0) 
        throw std::runtime_error("Cannot start server socket");

    int opt = 1;
    setsockopt(state.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(binding.port);
    inet_pton(AF_INET, binding.ip.c_str(), &addr.sin_addr);

    if (bind(state.fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(state.fd);
        throw std::runtime_error("Cannot bind server socket");
    }

    if (listen(state.fd, 16) < 0) {
        close(state.fd);
        throw std::runtime_error("Cannot listen to server socket");
    }

    int flags = fcntl(state.fd, F_GETFL, 0);
    fcntl(state.fd, F_SETFL, flags | O_NONBLOCK);

    state.role = Role::SERVER;
    state.active = true;
    state.serverThread = std::thread(acceptLogic, editor);
}

void closeServer(editor::TextEditor* editor) {
    NetworkingState& state = editor->networking;
    if (!state.active)
        return;

    state.active = false;
    shutdown(state.fd, SHUT_RDWR);
    close(state.fd);

    if (state.serverThread.joinable())
        state.serverThread.join();
}