#include "editor.h"

#include <cstring>
#include <random>
#include <unistd.h>

#include "ansi.h"
#include "events/event.h"
#include "events/keyboard_event.h"

namespace editor {

    TextEditor::TextEditor() {
        Window window = windowSize();
        writeStr(std::to_string(window.rows));
        writeStr(std::to_string(window.cols));

        this->state = State{
            window,
            PHILOSOPHICAL,
            Cursor{1, 0},
            {}
        };

        writeStr(ansi::SHOW_CURSOR);
    }

    TextEditor::~TextEditor() {
        this->closeFile();
    }

    void TextEditor::openFile(const std::string &path) {
        this->path = path;
        this->cake.loadFromFile(path);
    }

    void TextEditor::closeFile() {
        write(STDOUT_FILENO, ansi::SHOW_CURSOR, strlen(ansi::SHOW_CURSOR));
    }

    bool TextEditor::handle() {
        char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n == 0)
            return true;

        if (n == -1)
            return false;

        if (c == CTRL_KEY('q'))
            return false;

        event::KeyboardEvent event(c);
        event::EventDispatcher::instance().fire(event);

        return true;
    }


    void TextEditor::render() {
        writeStr(ansi::HIDE_CURSOR);
        writeStr(ansi::CURSOR_HOME);

        drawRows();
        drawStatusBar();
        moveCursor();

        writeStr(ansi::SHOW_CURSOR);
    }

    void TextEditor::drawRows() const {
        const int totalLines = static_cast<int>(this->cake.lineCount());
        const int numberWidth = std::to_string(totalLines).length();

        for (int y = 0; y < this->state.window.rows - 1; y++) {
            writeStr(ansi::CLEAR_LINE);

            if (y < totalLines) {
                const std::string content = cake.renderLine(y);

                std::string prefix;
                int numPad = numberWidth - std::to_string(y + 1).length();
                prefix.append(numPad, ' ');
                prefix += std::to_string(y + 1);
                prefix += " ";

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
                std::string(this->state.mode == WRITING ? "WRITING" : "PHILOSOPHICAL") +
                " | Ctrl-K toggle | Ctrl-Q quit ";

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
        const size_t screenX = std::to_string(this->cake.lineCount()).length() + 1 + this->state.cursor.x;
        const size_t screenY = this->state.cursor.y + 1;

        snprintf(buf, sizeof(buf), "\x1b[%lu;%luH", screenY, screenX);
        write(STDOUT_FILENO, buf, strlen(buf));
    }

    void TextEditor::pushUndo() {
        this->undoStack.push_back({
            this->cake.add,
            this->cake.lines,
            this->state.cursor
        });
        this->redoStack.clear();
    }

    void TextEditor::undo() {
        if (this->undoStack.empty())
            return;

        this->redoStack.push_back({
            this->cake.add,
            this->cake.lines,
            this->state.cursor
        });

        UndoState snapshot = undoStack.back();
        this->undoStack.pop_back();

        this->cake.add = snapshot.add;
        this->cake.lines = snapshot.lines;
        this->state.cursor = snapshot.cursor;
    }


    void TextEditor::redo() {
        if (redoStack.empty())
            return;

        undoStack.push_back({
            cake.add,
            cake.lines,
            state.cursor
        });

        auto snapshot = redoStack.back();
        redoStack.pop_back();

        cake.add = snapshot.add;
        cake.lines = snapshot.lines;
        state.cursor = snapshot.cursor;
    }

}
