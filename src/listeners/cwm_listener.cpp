#include "listeners/cwm_listener.h"

#include "net/networking.h"
#include "tui/alert.h"

namespace listener {

    NetworkBinding parseAddr(const std::string& addr);

    void CwmListener::handle(const event::CwmEvent& e) {
        NetworkingState& state = editor.state.networking;
        if (state.active && e.options == tui::CwmOptions::DISCONNECT) {
            state.active = false; // TODO close socket
            close(state.socket);
            return;
        }

        const tui::CwmOptions option = e.options;
        if (option == tui::CwmOptions::HOST) {
            const NetworkBinding binding = parseAddr(e.addr);
            const int fd = startSocket(binding, SERVER);
            if (fd < 0) {
                bool flag = false;
                while (!flag)
                    flag = tui::alert(editor.state.window, "CANNOT BIND A SOCKET", tui::ERROR);

                return;
            }

            state.binding = binding;
            state.socket = fd;
        }

        editor.state.networking.active = true;
    }

    NetworkBinding parseAddr(const std::string& addr) {
        const size_t colonPos = addr.find_last_of(':');
        const std::string ip = addr.substr(0, colonPos);
        const std::string portStr = addr.substr(colonPos + 1);

        int port = std::stoi(portStr);
        return NetworkBinding{
            ip,
            port,
            addr
        };
    }

}
