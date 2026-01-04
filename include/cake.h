#ifndef CAKE_H
#define CAKE_H

#include <fstream>
#include <string>
#include <vector>

namespace cake {

    enum class BufferKind {
        ORIGINAL,
        ADD
    };

    struct piece_t {
        BufferKind buf;
        size_t start;
        size_t len;
    };

    using Line = std::vector<piece_t>;

    class Cake {
    public:
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
    private:
        std::string original;
        std::string add;
        std::vector<Line> lines;
    };

}

#endif // CAKE_H