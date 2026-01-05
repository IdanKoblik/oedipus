#ifndef CONFIG_H
#define CONFIG_H

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
        KEYBINDING_COUNT
    };

    enum Setting {
        TAB,
        SETTING_COUNT
    };

    struct KeyBinding_t {
        KeyBinding key{};
        char shortcut{};
    };

    template<typename T>
    struct Setting_t {
        Setting setting{};
        T value{};
    };

    struct Config {
        std::array<KeyBinding_t, KEYBINDING_COUNT> keybindings;
        std::array<Setting_t<int>, SETTING_COUNT> settings;

        Config();
    };

    void load_config(const std::string& path, Config& cfg);
}

#endif // CONFIG_H