#ifndef PIECE_TABLE
#define PIECE_TABLE

#include <cstdio>
#include <vector>
#include <string>

namespace pieceTable  {

enum class BufferKind {
    ORIGINAL,
    ADD
};

struct piece_t {
    BufferKind buffer;
    size_t start;
    size_t len;
};

using Line = std::vector<piece_t>;

class Table {
public:
    std::string original;
    std::string add;
    std::vector<Line> lines;

    void loadFromFile(const std::string& path);
    void saveToFile(const std::string& path) const;

    std::string renderLine(size_t y) const;
    size_t lineCount() const;

    void insertChar(size_t x, size_t y, char c);
    void deleteChar(size_t x, size_t y);
    void insertNewLine(size_t x, size_t y);
    void removeLine(size_t y);

    std::vector<std::string> getLines() const;
    
    size_t lineLength(size_t y) const;
};

}

#endif // PIECE_TABLE