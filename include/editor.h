 #ifndef EDITOR_H
#define EDITOR_H

#include <string>
#include <memory>
#include "config/config.h"
#include "context.h"
#include "terminal.h"
#include "structs/piece_table.h"
#include "cursor.h"
#include "algorithem/search.h"

namespace editor {

enum class Mode {
    PHILOSOPHICAL,
    WRITING
};

struct UndoState {
    std::string add;
    std::vector<pieceTable::Line> lines;
    Cursor cursor;
};

struct State {
    Window window{};
    Mode mode;
    Cursor cursor{};
    SearchState searchState{};
    SearchState search{};
};

class TextEditor {
public:
    std::unique_ptr<Context> ctx;
    State state{};
    config::Config cfg;
    pieceTable::Table pieceTable;
    std::string filePath;

    std::vector<UndoState> undoStack;
    std::vector<UndoState> redoStack;

    TextEditor(const config::Config& cfg, std::unique_ptr<Context> c);
    ~TextEditor();

    void openFile(const std::string& path);
    void closeFile();

    void render();
    bool handle();


    void undo();
    void redo();
    void pushUndo();

    TextEditor(const TextEditor&) = delete;
    TextEditor& operator=(const TextEditor&) = delete;
private:
    void drawRows() const;
    void moveCursor() const;
    void drawStatusBar() const;
};

}

#endif // EDITOR_H