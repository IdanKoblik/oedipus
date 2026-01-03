#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <array>

namespace config {
    enum KeyBinding {
        UNDO,
        REDO,
        SEARCH_MOVE,
        SWITCH_MOVE,
        KEYBINDING_COUNT
    };

    enum Setting {
        TAB,
        SETTING_COUNT
    };

    struct keyBinding_t {
        KeyBinding key{};
        char shortcut{};
    };

    template<typename T>
    struct setting_t {
        Setting setting{};
        T value{};
    };

    struct config_t {
        std::array<keyBinding_t, KEYBINDING_COUNT> keybindings;
        std::array<setting_t<int>, SETTING_COUNT> settings;

        config_t();
    };

    void load_config(const std::string& path, config_t& cfg);
}

#endif // SETTINGS_H