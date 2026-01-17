#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <klogger/logger.h>
#include <termios.h>
#include "terminal.h"
#include "ansi.h"
#include "editor.h"
#include "config/config.h"

static const std::string CONFIG_PATH = std::string(std::getenv("HOME")) + "/.config/oedipus/config.ini";

struct {
    struct termios term;
    int initialized = 0;
} term_;

void shutdownWrapper(void);
void signalHandler(int sig);
void registerSignals();

int main(int argc, char** argv) {   
    klogger::Logger::instance().init("logs.txt");
    LOG_INFO("Oedipus editor starting");

    config::Config cfg{};
    try {
        config::load_config(CONFIG_PATH, cfg);
        LOG_INFO("Configuration loaded successfully");
    } catch (const std::exception& e) {
        LOG_FATAL("Failed to load configuration: " + std::string(e.what()));
        return 1;
    }

    enableRawMode(&term_.term);
    term_.initialized = 1;

    atexit(shutdownWrapper);
    registerSignals();

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);

    writeStr(ansi::ENABLE_ALT_BUFFER);
    LOG_DEBUG("Terminal raw mode enabled");

    std::unique_ptr<Context> ctx = std::make_unique<Context>();
    std::string file;
    if (argc != 2) {
        NetworkBinding bind = NetworkBinding{"127.0.0.1", 9090, "127.0.0.1:9090"};
        ctx->startClient(bind);
        ctx->clientRef().wait();
        if (ctx->hasClient())
            file = ctx->clientRef().downloadFile();
    } else
        file = argv[1];

    LOG_INFO("Opening file: " + file);

    editor::TextEditor editor(cfg, std::move(ctx));
    editor.openFile(file);

    int exitCode = 0;
    try {
        while (true) {
            editor.render();

            if (!editor.handle())
                break;
        }
    } catch (const std::exception& e) {
        LOG_EXCEPTION(e);
        LOG_FATAL("Program crashed due to exception: " + std::string(e.what()));
        exitCode = 1;
    } catch (...) {
        LOG_UNKNOWN_EXCEPTION();
        LOG_FATAL("Program crashed due to unknown exception");
        exitCode = 1;
    }

    klogger::Logger::instance().shutdown();
    return exitCode;
}

void shutdownWrapper(void) {
    if (term_.initialized) {
        writeStr(ansi::DISABLE_ALT_BUFFER);
        disableRawMode(&term_.term);
        term_.initialized = 0;
    }
}

void signalHandler(int sig) {
    shutdownWrapper();
    _exit(128 + sig);
}

void registerSignals() {
    struct sigaction sa{};
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESETHAND;  // Reset handler to default after first call

    sigaction(SIGINT,  &sa, nullptr); // Ctrl+C
    sigaction(SIGTERM, &sa, nullptr); // kill
    sigaction(SIGHUP,  &sa, nullptr); // terminal hangup
    sigaction(SIGSEGV, &sa, nullptr); // segmentation fault
    sigaction(SIGABRT, &sa, nullptr); // abort
    sigaction(SIGFPE,  &sa, nullptr); // arithmetic error
    sigaction(SIGILL,  &sa, nullptr); // illegal instruction
}
