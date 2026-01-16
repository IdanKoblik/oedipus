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
#include "utils/logger.h"
#include "proto/editor.pb.h"
#include <google/protobuf/message.h>
#include "net/server.h"
#include "context.h"
#include <thread>

static const std::string CONFIG_PATH = std::string(std::getenv("HOME")) + "/.config/oedipus/config.ini";

void closeEditor(struct termios *term);
void registerListeners(editor::TextEditor& editor);
std::string ask(Context& ctx);

int main(int argc, char **argv) {
    // Initialize logger first
    logger::Logger::instance().init("logs.txt");
    LOG_INFO("Oedipus editor starting");
    
    int exitCode = 0;
    struct termios term;
    bool termInitialized = false;
    
    try {
        std::cout << CONFIG_PATH << std::endl;
        LOG_DEBUG("Config path: " + CONFIG_PATH);

        config::Config cfg{};
        try {
            config::load_config(CONFIG_PATH, cfg);
            LOG_INFO("Configuration loaded successfully");
        } catch (const std::exception& e) {
            LOG_FATAL("Failed to load configuration: " + std::string(e.what()));
            return 1;
        }

        enableRawMode(&term);
        termInitialized = true;
        writeStr("\x1b[?1049h");
        LOG_DEBUG("Terminal raw mode enabled");

        Context ctx;
        std::string file;
        if (argc != 2) {
            std::string str = ask(ctx);
            if (str.empty()) {
                closeEditor(&term);
                LOG_INFO("Editor closed by user (no file selected)");
                return 0;
            }

            file = str;
        } else {
            file = argv[1];
        }
        
        LOG_INFO("Opening file: " + file);

        editor::TextEditor editor(cfg, &ctx);
        registerListeners(editor);
        LOG_DEBUG("Event listeners registered");

        editor.openFile(file);
        LOG_INFO("File opened successfully");
        
        while (true) {
            editor.render();

            if (!editor.handle())
                break;
        }
        
        LOG_INFO("Editor main loop exited normally");

    } catch (const std::exception& e) {
        LOG_EXCEPTION(e);
        LOG_FATAL("Program crashed due to exception: " + std::string(e.what()));
        exitCode = 1;
    } catch (...) {
        LOG_UNKNOWN_EXCEPTION();
        LOG_FATAL("Program crashed due to unknown exception");
        exitCode = 1;
    }
    
    // Cleanup
    if (termInitialized) {
        try {
            closeEditor(&term);
        } catch (...) {
            LOG_ERROR("Error during terminal cleanup");
        }
    }
    
    logger::Logger::instance().shutdown();
    return exitCode;
}

void closeEditor(struct termios *term) {
    writeStr("\x1b[?1049l");
    disableRawMode(term);
}

void registerListeners(editor::TextEditor& editor) {
    event::EventDispatcher& dispatcher = event::EventDispatcher::instance();

    static listener::KeyboardListener keyboardListener(editor, *editor.ctx);
    dispatcher.registerListener(&keyboardListener);

    static listener::MovementListener movementListener(editor);
    dispatcher.registerListener(&movementListener);

    static listener::ModeListener modeListener(editor);
    dispatcher.registerListener(&modeListener);

    static listener::SearchListener searchListener(editor);
    dispatcher.registerListener(&searchListener);
}

std::string ask(Context& ctx) {
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
                LOG_INFO("Connecting to: " + addr);
            } catch (const std::exception& e) {
                LOG_EXCEPTION(e);
                LOG_ERROR("Failed to parse network address: " + addr);
                return std::string{};
            } catch (...) {
                LOG_UNKNOWN_EXCEPTION();
                LOG_ERROR("Unknown error parsing network address: " + addr);
                return std::string{};
            }

            try {
                ctx.startClient(bind);
                if (!ctx.clientRef().active) {
                    LOG_WARN("Client connection failed");
                    return std::string{};
                }
                
                LOG_INFO("Client connected successfully");
                std::string downloadedFile = ctx.clientRef().downloadFile();
                if (!downloadedFile.empty()) {
                    LOG_INFO("File downloaded to: " + downloadedFile);
                } else {
                    LOG_WARN("File download returned empty path");
                }
                return downloadedFile;
            } catch (const std::exception& e) {
                LOG_EXCEPTION(e);
                LOG_ERROR("Error during client connection: " + std::string(e.what()));
                return std::string{};
            } catch (...) {
                LOG_UNKNOWN_EXCEPTION();
                LOG_ERROR("Unknown error during client connection");
                return std::string{};
            }
        }

    }

    return std::string{};
}
