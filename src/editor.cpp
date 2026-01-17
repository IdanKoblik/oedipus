#include "editor.h"

#include "ansi.h"
#include "global.h"
#include "search.h"
#include "tui/tui.h"
#include "tui/alert.h"
#include "handlers/keyboard_handler.h"
#include <unistd.h>
#include <string>
#include <cstring>
#include <filesystem>
#include <klogger/logger.h>

namespace editor {

TextEditor::TextEditor(const config::Config& cfg, std::unique_ptr<Context> c) {
    this->cfg = cfg;
    this->ctx = std::move(c);

    Window window = windowSize();
    writeStr(std::to_string(window.rows));
    writeStr(std::to_string(window.cols));

    this->state = State{
        window,
        Mode::PHILOSOPHICAL,
        Cursor{1, 0},
        {},
        {}
    };

    writeStr(ansi::SHOW_CURSOR);
}

TextEditor::~TextEditor() {
    this->closeFile();
}

void TextEditor::openFile(const std::string &path) {
    this->filePath = path;
    this->pieceTable.loadFromFile(path);
}

void TextEditor::closeFile() {
    write(STDOUT_FILENO, ansi::SHOW_CURSOR, std::strlen(ansi::SHOW_CURSOR));
}

bool TextEditor::handle() {
    char c;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (n == 0)
        return true;

    if (n == -1)
        return false;

    if (c == CTRL_KEY('k')) {
        editor::Mode mode = (this->state.mode == Mode::WRITING)
            ? Mode::PHILOSOPHICAL
            : Mode::WRITING;
            
        if (mode == Mode::PHILOSOPHICAL) 
            this->pieceTable.saveToFile(this->filePath);
        
        this->state.search = {};
        this->state.mode = mode;

        return true;
    }

    if (this->state.mode == Mode::PHILOSOPHICAL) {
        if (c == CTRL_KEY('q'))
            return false;

        handlePhilosophicalMode(c, this);
    }

    if (this->state.mode == Mode::WRITING) 
        handleWritingMode(c, this);

    return true;
}

void TextEditor::render() {
    writeStr(ansi::HIDE_CURSOR);
    writeStr(ansi::CURSOR_HOME);
    
    this->drawRows();
    this->drawStatusBar();
    this->moveCursor();
    
    writeStr(ansi::SHOW_CURSOR);
}

void TextEditor::drawRows() const {
    const int totalLines = static_cast<int>(this->pieceTable.lineCount());
    const int numberWidth = std::to_string(totalLines).length();

    for (int y = 0; y < this->state.window.rows - 1; y++) {
        writeStr(ansi::CLEAR_LINE);

        if (y < totalLines) {
            const std::string content = this->pieceTable.renderLine(y);
            std::string prefix;
            int numPad = numberWidth - std::to_string(y + 1).length();
            prefix.append(numPad, ' ');
            prefix += std::to_string(y + 1);
            prefix += " ";

            auto it = this->state.searchState.matches.find(y);
            if (this->state.searchState.active && it != this->state.searchState.matches.end()) {
                write(STDOUT_FILENO, prefix.data(), prefix.size());
                tui::drawLine(
                    content,
                    it->second,
                    this->state.searchState.targetSize
                );
                continue;
            }
            std::string line = prefix + content;
            write(STDOUT_FILENO, line.data(), line.size());
        }

        writeStr(ansi::CRLF);
    }
}

void TextEditor::drawStatusBar() const {
    writeStr(ansi::INVERT_COLORS);
    std::string status =
            " oedipus | " +
            std::string(this->state.mode == Mode::WRITING ? "WRITING" : "PHILOSOPHICAL") +
            " | Ctrl-K toggle | Ctrl-Q quit ";

    if (this->ctx->hasServer()) 
        status += " | Server running on: " + this->ctx->serverRef().binding.addr;

    const size_t cols = this->state.window.cols;
    if (static_cast<int>(status.size()) > cols)
        status.resize(cols);

    write(STDOUT_FILENO, status.c_str(), status.size());
    while (static_cast<int>(status.size()) < cols) {
        write(STDOUT_FILENO, " ", 1);
        status.push_back(' ');
    }

    writeStr(ansi::RESET_ATTRS);
}

void TextEditor::moveCursor() const {
    char buf[32];

    // +1 for the immutable space after line number
    const size_t screenX = std::to_string(this->pieceTable.lineCount()).length() + 1 + this->state.cursor.x;
    const size_t screenY = this->state.cursor.y + 1;

    snprintf(buf, sizeof(buf), "\x1b[%lu;%luH", screenY, screenX);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void TextEditor::pushUndo() {
    this->undoStack.push_back({
        this->pieceTable.add,
        this->pieceTable.lines,
        this->state.cursor
    });
    this->redoStack.clear();
}

void TextEditor::undo() {
    if (this->undoStack.empty())
        return;

    this->redoStack.push_back({
        this->pieceTable.add,
        this->pieceTable.lines,
        this->state.cursor
    });

    UndoState snapshot = this->undoStack.back();
    this->undoStack.pop_back();
    this->pieceTable.add = snapshot.add;
    this->pieceTable.lines = snapshot.lines;
    this->state.cursor = snapshot.cursor;
}

void TextEditor::redo() {
    if (this->redoStack.empty())
        return;

    this->undoStack.push_back({
        this->pieceTable.add,
        this->pieceTable.lines,
        this->state.cursor
    });

    auto snapshot = this->redoStack.back();
    this->redoStack.pop_back();
    this->pieceTable.add = snapshot.add;
    this->pieceTable.lines = snapshot.lines;
    this->state.cursor = snapshot.cursor;
}

}