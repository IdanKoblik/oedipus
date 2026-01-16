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
#include "context.h"
#include <thread>

static const std::string CONFIG_PATH = std::string(std::getenv("HOME")) + "/.config/oedipus/config.ini";

void closeEditor(struct termios *term);
void registerListeners(editor::TextEditor& editor);
std::string ask(Context& ctx);

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

    Context ctx;
    std::string file;
    if (argc != 2) {
        std::string str = ask(ctx);
        if (str.empty()) {
            closeEditor(&term);
            return 0;
        }

        file = str;
    } else file = argv[1];

    editor::TextEditor editor(cfg, &ctx);
    registerListeners(editor);

    editor.openFile(file);
    while (true) {
        editor.render();

        if (!editor.handle())
            break;
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
            } catch (...) {
                return std::string{};
            }

            ctx.startClient(bind);
            if (!ctx.clientRef().active)
                return std::string{};

            return ctx.clientRef().downloadFile();
        }

    }

    return std::string{};
}
