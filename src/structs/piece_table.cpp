#include "structs/piece_table.h" 

#include <fstream>
#include <iterator>

namespace pieceTable {

void Table::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    this->original.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );

    this->lines.clear();
    this->add.clear();

    size_t start = 0;
    for (size_t i = 0; i <= this->original.size(); i++) {
        if (i == this->original.size() || this->original[i] == '\n') {
            this->lines.push_back({
                piece_t{BufferKind::ORIGINAL, start, i - start}
            });

            start = i + 1;
        }
    }

    if (this->lines.empty())
        this->lines.push_back(Line{});
}

void Table::saveToFile(const std::string& path) const {
    std::ofstream out(path);

    for (size_t y = 0; y < this->lines.size(); y++) {
        for (const auto& p : lines[y]) {
            const std::string& src = (p.buffer == BufferKind::ORIGINAL) ? this->original : this->add;
            out.write(src.data() + p.start, p.len);
        }

        if (y + 1 < this->lines.size())
            out.put('\n');
    }
}

std::string Table::renderLine(size_t y) const {
    std::string out;
    for (const auto& p : lines[y]) {
        const std::string& src = (p.buffer == BufferKind::ORIGINAL) ? this->original : this->add;
        out.append(src, p.start, p.len);
    }

    return out;
}

size_t Table::lineCount() const {
    return this->lines.size();
}

size_t Table::lineLength(size_t y) const {
    size_t len = 0;
    for (const auto& p : lines[y])
        len += p.len;

    return len;
}

void Table::insertChar(size_t x, size_t y, char c) {
    size_t addPos = this->add.size();
    this->add.push_back(c);

    Line& line = this->lines[y];
    size_t acc = 0;

    for (size_t i = 0; i <= line.size(); ++i) {
        bool atEnd = (i == line.size());
        if (!atEnd && x > acc + line[i].len) {
            acc += line[i].len;
            continue;
        }

        if (!atEnd) {
            piece_t& p = line[i];
            size_t off = x - acc;

            std::vector<piece_t> newPieces;
            if (off > 0)
                newPieces.push_back({p.buffer, p.start, off});

            newPieces.push_back({BufferKind::ADD, addPos, 1});
            if (off < p.len)
                newPieces.push_back({p.buffer, p.start + off, p.len - off});

            line.erase(line.begin() + i);
            line.insert(line.begin() + i, newPieces.begin(), newPieces.end());
        } else {
            line.push_back({BufferKind::ADD, addPos, 1});
        }
        break;
    }
}

void Table::deleteChar(size_t x, size_t y) {
    if (x == 0)
        return;

    Line& line = this->lines[y];
    size_t target = x -1;
    size_t acc = 0;

    for (size_t i = 0; i < line.size(); ++i) {
        piece_t& p = line[i];

        if (target < acc + p.len) {
            size_t off = target - acc;
            piece_t left = {p.buffer, p.start, off};
            piece_t right = {p.buffer, p.start + off + 1, p.len - off - 1};
        
            line.erase(line.begin() + i);
            if (right.len)
                line.insert(line.begin() + i, right);

            if (left.len)
                line.insert(line.begin() + i, left);

            break;
        }

        acc += p.len;
    }
}

void Table::insertNewLine(size_t x, size_t y) {
    Line& line = this->lines[y];
    Line left, right;

    size_t acc = 0;
    for (const auto& p : line) {
        if (acc + p.len <= x) {
            left.push_back(p);
        } else if (acc >= x) {
            right.push_back(p);
        } else {
            size_t off = x - acc;
            left.push_back({p.buffer, p.start, off});
            right.push_back({p.buffer, p.start + off, p.len - off});
        }

        acc += p.len;
    }

    this->lines[y] = std::move(left);
    this->lines.insert(this->lines.begin() + y + 1, std::move(right));
}

void Table::removeLine(size_t y) {
    if (this->lines.empty())
        return;

    if (this->lines.size() == 1) {
        lines[0].clear();
        return;
    }

    if (y == 0) {
        Line& first = this->lines[0];
        Line& second = this->lines[1];

        second.insert(second.begin(), first.begin(), first.end());
        
        this->lines.erase(lines.begin());
        return;
    }

    Line& prev = this->lines[y - 1];
    Line& curr = this->lines[y];

    prev.insert(prev.end(), curr.begin(), curr.end());
    this->lines.erase(lines.begin() + y);
}

std::vector<std::string> Table::getLines() const {
    std::vector<std::string> out;
    out.reserve(this->lines.size());
    
    for (size_t y = 0; y < this->lines.size(); ++y)
        out.push_back(this->renderLine(y));
    
    return out;
}

}