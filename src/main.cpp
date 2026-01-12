#include <filesystem>
#include <termios.h>

#include "editor.h"
#include "terminal.h"
#include "events/event.h"
#include "listeners/keyboard_listener.h"
#include "listeners/mode_listener.h"
#include "listeners/movement_listener.h"
#include "listeners/search_listener.h"
#include "tui/alert.h"
#include "tui/menu.h"
#include "net/networking.h"
#include "net/client.h"
#include "tui/prompt.h"
#include "utils/ip.h"
#include "proto/editor.pb.h"
#include <google/protobuf/message.h>
#include "net/server.h"
#include <thread>

static const std::string CONFIG_PATH = std::string(std::getenv("HOME")) + "/.config/oedipus/config.ini";

void closeEditor(struct termios *term);
void registerListeners(editor::TextEditor& editor);
std::string ask();

int main(int argc, char **argv) {
    std::cout << CONFIG_PATH << std::endl;

    config::Config cfg{};
    try {
        config::load_config(CONFIG_PATH, cfg);
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << '\n';
        return 0;
    }

    struct termios term;
    enableRawMode(&term);
    writeStr("\x1b[?1049h");

    std::string file;
    if (argc != 2) {
        std::string str = ask();
        if (str.empty()) {
            closeEditor(&term);
            return 0;
        }

        file = str;
    } else file = argv[1];

    editor::TextEditor editor(cfg);
    registerListeners(editor);

    editor.openFile(file);
    while (true) {
        editor.render();

        if (!editor.handle())
            break;

        if (editor.networking.active && editor.networking.role == Role::SERVER) {
            sockaddr_in clientAddr{};
            socklen_t len = sizeof(clientAddr);
            int clientFd = accept(editor.networking.fd, (sockaddr*)&clientAddr, &len);
            if (clientFd < 0) {
                perror("accept");
                continue;
            }

            oedipus::ConnectReq req;
            if (!recvProto(clientFd, req)) {
                std::cerr << "Failed to receive ConnectReq\n";
                close(clientFd);
                continue;
            }

            oedipus::ConnectAck ack;
            ack.set_ok(true);
            ack.set_message("Connected");
            if (!sendProto(clientFd, ack)) {
                std::cerr << "Failed to send ConnectAck\n";
                close(clientFd);
                continue;
            }

            if (!sendFile(clientFd, editor.path, req.clientid())) {
                std::cerr << "Failed to send file\n";
                close(clientFd);
                continue;
            }

            shutdown(clientFd, SHUT_WR);
            close(clientFd);
            std::cout << "File sent successfully\n";
        }
    }

    closeEditor(&term);
    return 0;
}

void closeEditor(struct termios *term) {
    writeStr("\x1b[?1049l");
    disableRawMode(term);
}

void registerListeners(editor::TextEditor& editor) {
    event::EventDispatcher& dispatcher = event::EventDispatcher::instance();

    static listener::KeyboardListener keyboardListener(editor);
    dispatcher.registerListener(&keyboardListener);

    static listener::MovementListener movementListener(editor);
    dispatcher.registerListener(&movementListener);

    static listener::ModeListener modeListener(editor);
    dispatcher.registerListener(&modeListener);

    static listener::SearchListener searchListener(editor);
    dispatcher.registerListener(&searchListener);
}

std::string runCwmClient(
    Window window,
    NetworkBinding& bind
) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return std::string{};

    sockaddr_in sock{};
    memset(&sock, 0, sizeof(sock));
    sock.sin_family = AF_INET;
    sock.sin_port = htons(bind.port);
    if (inet_pton(AF_INET, bind.ip.c_str(), &sock.sin_addr) != 1) {
        close(fd);
        return std::string{};
    }

    if (connect(fd, reinterpret_cast<sockaddr*>(&sock), sizeof(sock)) < 0) {
        close(fd);
        return std::string{};
    }
    
    oedipus::ConnectReq req;
    req.set_clientid(1);

    if (!sendProto(fd, req)) {
        tui::alert(window, "Failed to send ConnectReq", tui::AlertType::ERROR);
        close(fd);
        return std::string{};
    }

    oedipus::ConnectAck ack;
    if (!recvProto(fd, ack) || !ack.ok()) {
        tui::alert(window, "Server rejected connection", tui::AlertType::ERROR);
        close(fd);
        return std::string{};
    }

    const std::string file = downloadFile(fd);
    close(fd);

    tui::alert(window, "File received successfully", tui::AlertType::INFO);
    return file;
}

std::string ask() {
    const Window window = windowSize();
    const auto option = tui::showMenu<tui::FileOptions>(window, "File Options", {
        {tui::FileOptions::OPEN_FILE, "Open File" },
        {tui::FileOptions::CREATE_FILE, "Create File" },
        {tui::FileOptions::CONNECT, "Connect" },
        {tui::FileOptions::EXIT, "Exit" }
    });

    switch (option) {
        case tui::FileOptions::EXIT: {
            break;
        }
        case tui::FileOptions::OPEN_FILE: {
            const std::string file = tui::prompt(window, "Open file", "Enter file path:");
            if (std::filesystem::exists(file))
                return file;

            bool flag = false;
            while (!flag)
                flag = tui::alert(window, "File not found", tui::ERROR);

            return std::string{};
        }
        case tui::FileOptions::CREATE_FILE: {
            const std::string file = tui::prompt(window, "Create new file", "Enter file name:");
            if (!std::filesystem::exists(file))
                return file;

            bool flag = false;
            while (!flag)
                flag = tui::alert(window, "File already exists", tui::ERROR);

            return std::string{};
        }
        case tui::FileOptions::CONNECT: {
            const std::string addr = tui::prompt(window, "CONNECT", "Enter host addr:");
            NetworkBinding bind{};

            try {
                bind = utils::extractBinding(addr);
            } catch (...) {
                return {};
            }

            return runCwmClient(window, bind);
        }

    }

    return std::string{};
}
