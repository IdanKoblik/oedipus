#include "config/config.h"

#include "editor.h"
#include "utils/logger.h"

namespace config {

    static std::string trim(std::string s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            [](unsigned char c) { return !std::isspace(c); }));
        s.erase(std::find_if(s.rbegin(), s.rend(),
            [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
        return s;
    }

    static char parse_key(const std::string& s) {
        std::string key = trim(s);
        const std::string leader_prefix = "$leader,";
        if(key.find(leader_prefix) == 0) {
            std::string after = trim(key.substr(leader_prefix.size()));
            if(!after.empty())
                return CTRL_KEY(after[0]);
        }
        if(!key.empty())
            return key[0];
        return 0;
    }

    Config::Config() {
        keybindings[UNDO]        = { UNDO,        'u' };
        keybindings[REDO]        = { REDO,        'r' };
        keybindings[SEARCH_MOVE] = { SEARCH_MOVE, '/' };
        settings[TAB] = { TAB, 4 };
    }

    void load_config(const std::string& path, Config& cfg) {
        LOG_DEBUG("Loading config from: " + path);
        std::ifstream file(path);
        if (!file.is_open()) {
            LOG_ERROR("Cannot open config file: " + path);
            throw std::runtime_error("Cannot find config file");
        }

        enum class Section { NONE, KEYBINDING, OPTIONS };
        Section current = Section::NONE;
        std::string line;

        const std::unordered_map<std::string, KeyBinding> keybinding_map = {
            {"UNDO", UNDO},
            {"REDO", REDO},
            {"SEARCH_MOVE", SEARCH_MOVE},
        };

        const std::unordered_map<std::string, Setting> setting_map = {
            {"TAB", TAB},
        };

        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;
            if (line == "[KEYBINDING]") { current = Section::KEYBINDING; continue; }
            if (line == "[OPTIONS]") { current = Section::OPTIONS; continue; }

            auto eq = line.find('=');
            if (eq == std::string::npos) continue;

            std::string key = trim(line.substr(0, eq));
            std::string value = trim(line.substr(eq + 1));

            if (current == Section::KEYBINDING) {
                auto it = keybinding_map.find(key);
                if (it != keybinding_map.end()) {
                    size_t idx = static_cast<size_t>(it->second);
                    cfg.keybindings[idx].shortcut = parse_key(value);
                }
            }

            if (current == Section::OPTIONS) {
                auto it = setting_map.find(key);
                if (it != setting_map.end()) {
                    size_t idx = static_cast<size_t>(it->second);
                    cfg.settings[idx].value = std::stoi(value);
                }
            }
        }
        LOG_DEBUG("Config file loaded successfully");
    }

}
