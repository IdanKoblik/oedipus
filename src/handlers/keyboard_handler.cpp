#include "handlers/keyboard_handler.h"

#include <cstring>
#include "global.h"
#include "config/config.h"
#include "tui/alert.h"
#include "tui/menu.h"
#include "tui/prompt.h"
#include "utils/ip.h"

#define MOVE_SIZE 4

static const char movementBinds[MOVE_SIZE] = {'h', 'j', 'k', 'l'};

static void handleMovement(char c, Cursor& cursor, pieceTable::Table& pieceTable) {
    switch (c) {
        case 'h': {
            if (cursor.x > 1) {
                cursor.x--;
            } else if (cursor.y > 0) {
                cursor.y--;
                cursor.x = pieceTable.lineLength(cursor.y) + 1;
            }
            break;
        }
        case 'l': {
            size_t len = pieceTable.lineLength(cursor.y) + 1;
            if (cursor.x < static_cast<int>(len)) {
                cursor.x++;
            } else if (cursor.y + 1 < static_cast<int>(pieceTable.lineCount())) {
                cursor.y++;
                cursor.x = 1;
            }
            break;
        }
        case 'k': {
            if (cursor.y > 0) {
                cursor.y--;
            }
            break;
        }
        case 'j': {
            if (cursor.y + 1 < static_cast<int>(pieceTable.lineCount()))
                cursor.y++;
            break;
        }
    }
    
    const size_t len = pieceTable.lineLength(cursor.y);
    if (cursor.x > static_cast<int>(len) + 1)
        cursor.x = len + 1;
}

static void handleSearch(char c, SearchState& searchState, pieceTable::Table& pieceTable, const std::string& target) {
    const std::vector<std::string> lines = pieceTable.getLines();
    
    for (size_t y = 0; y < lines.size(); ++y) {
        const std::string& line = lines[y];
        std::vector<size_t> res = search::kmp(line, target);
        if (res.empty())
            continue;

        searchState.active = true;
        searchState.target = target;
        searchState.matches[y] = res;
        searchState.positions.push_back(Cursor{res[0], y});
        searchState.targetSize = strlen(target.c_str());
    }
}

void handleWritingMode(char c, editor::TextEditor* editor) {
    if (editor == nullptr)
        return;

    if (editor->state.mode != editor::Mode::WRITING)
        return;

    Cursor& cursor = editor->state.cursor;
    pieceTable::Table& pieceTable = editor->pieceTable;

    if (isprint(c)) {
        editor->pushUndo();
        editor->redoStack.clear();
        pieceTable.insertChar(cursor.x - 1, cursor.y, c);
        cursor.x++;
        return;
    }

    if (c == BACKSPACE || c == '\b') {
        if (cursor.x > 1 || cursor.y > 0) {
            editor->pushUndo();
            editor->redoStack.clear();
        }
        if (cursor.x > 1) {
            pieceTable.deleteChar(cursor.x - 1, cursor.y);
            cursor.x--;
            return;
        }
        if (cursor.y > 0) {
            pieceTable.removeLine(cursor.y);
            cursor.y--;
            cursor.x = pieceTable.lineLength(cursor.y) + 1;
        }
        return;
    }


    if (c == '\r') {
        editor->pushUndo();
        editor->redoStack.clear();

        pieceTable.insertNewLine(cursor.x - 1, cursor.y);

        cursor.y++;
        cursor.x = 1;
        return;
    }

    if (c == '\t') {
        editor->pushUndo();
        editor->redoStack.clear();

        for (int i = 0; i < editor->cfg.settings[config::TAB].value; i++) {
            pieceTable.insertChar(cursor.x - 1, cursor.y, ' ');
            cursor.x++;
        }

        return;
    }
}

void handlePhilosophicalMode(char c, editor::TextEditor* editor) {
    if (editor == nullptr)
        return;

    if (editor->state.mode != editor::Mode::PHILOSOPHICAL)
        return;
    
    if (c == editor->cfg.keybindings[config::CWM_MENU].shortcut) {
        if (editor->ctx->hasServer() && editor->ctx->serverRef().active) {
            const auto option = tui::showMenu<tui::CwmOptions>(editor->state.window, "Code with me", {
                {tui::CwmOptions::DISCONNECT, "DISCONNECT" },
            });

            if (option == tui::CwmOptions::DISCONNECT) {
                editor->ctx->stopServer();
                return;
            }

            return;
        }

        const std::string addr = tui::prompt(editor->state.window, "Open code with me", "Enter server addr:");
        try {
            const NetworkBinding binding = utils::extractBinding(addr);
            editor->ctx->startServer(binding, editor->filePath);
            editor->ctx->serverRef().wait();
        } catch (...) {
            tui::alert(editor->state.window, "Failed to start code with me", tui::AlertType::ERROR);
            return;
        }
        
        return;
    }

    if (c == editor->cfg.keybindings[config::SEARCH_MOVE].shortcut) {
        const std::string target = tui::prompt(editor->state.window, "Search", "Enter target to search:");
        handleSearch(c, editor->state.search, editor->pieceTable, target);

        return;
    }

    if (c == editor->cfg.keybindings[config::UNDO].shortcut) {
        editor->undo();
        return;
    }

    if (c == editor->cfg.keybindings[config::REDO].shortcut) {
        editor->redo();
        return;
    }

    for (char bind : movementBinds) {
        if (c == bind) {
            handleMovement(c, editor->state.cursor, editor->pieceTable);
            break;
        }
    }
}