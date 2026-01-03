#include "cake.h"

#include <fstream>
#include <iterator>

#include "editor.h"

namespace cake {

    void Cake::loadFromFile(const std::string& path) {
        std::ifstream file(path);
        original.assign(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );

        lines.clear();
        add.clear();

        size_t start = 0;
        for (size_t i = 0; i <= original.size(); ++i) {
            if (i == original.size() || original[i] == '\n') {
                lines.push_back({
                    piece_t{BufferKind::ORIGINAL, start, i - start}
                });
                start = i + 1;
            }
        }

        if (lines.empty())
            lines.push_back(Line{});
    }

    void Cake::saveToFile(const std::string& path) const {
        std::ofstream out(path);

        for (size_t y = 0; y < lines.size(); ++y) {
            for (const auto& p : lines[y]) {
                const std::string& src =
                    (p.buf == BufferKind::ORIGINAL) ? original : add;
                out.write(src.data() + p.start, p.len);
            }
            if (y + 1 < lines.size())
                out.put('\n');
        }
    }

    std::string Cake::renderLine(size_t y) const {
        std::string out;
        for (const auto& p : lines[y]) {
            const std::string& src =
                (p.buf == BufferKind::ORIGINAL) ? original : add;
            out.append(src, p.start, p.len);
        }
        return out;
    }

    size_t Cake::lineCount() const {
        return lines.size();
    }

    size_t Cake::lineLength(size_t y) const {
        size_t len = 0;
        for (const auto& p : lines[y])
            len += p.len;
        return len;
    }

    void Cake::insertChar(size_t x, size_t y, char c) {
        size_t addPos = add.size();
        add.push_back(c);

        Line& line = lines[y];
        size_t acc = 0;

        for (size_t i = 0; i <= line.size(); ++i) {
            if (i == line.size() || x <= acc + line[i].len) {
                if (i < line.size()) {
                    piece_t& p = line[i];
                    size_t off = x - acc;

                    piece_t left  = {p.buf, p.start, off};
                    piece_t right = {p.buf, p.start + off, p.len - off};

                    line.erase(line.begin() + i);
                    if (right.len) line.insert(line.begin() + i, right);
                    line.insert(line.begin() + i, {BufferKind::ADD, addPos, 1});
                    if (left.len)  line.insert(line.begin() + i, left);
                } else {
                    line.push_back({BufferKind::ADD, addPos, 1});
                }
                break;
            }
            acc += line[i].len;
        }
    }

    void Cake::deleteChar(size_t x, size_t y) {
        if (x == 0) return;

        Line& line = lines[y];
        size_t target = x - 1;
        size_t acc = 0;

        for (size_t i = 0; i < line.size(); ++i) {
            piece_t& p = line[i];

            if (target < acc + p.len) {
                size_t off = target - acc;

                piece_t left  = {p.buf, p.start, off};
                piece_t right = {p.buf, p.start + off + 1, p.len - off - 1};

                line.erase(line.begin() + i);
                if (right.len) line.insert(line.begin() + i, right);
                if (left.len)  line.insert(line.begin() + i, left);
                break;
            }
            acc += p.len;
        }
    }

    void Cake::insertNewLine(size_t x, size_t y) {
        Line& line = lines[y];
        Line left, right;

        size_t acc = 0;
        for (const auto& p : line) {
            if (acc + p.len <= x) {
                left.push_back(p);
            } else if (acc >= x) {
                right.push_back(p);
            } else {
                size_t off = x - acc;
                left.push_back({p.buf, p.start, off});
                right.push_back({p.buf, p.start + off, p.len - off});
            }
            acc += p.len;
        }

        lines[y] = std::move(left);
        lines.insert(lines.begin() + y + 1, std::move(right));
    }

    void Cake::removeLine(size_t y) {
        if (lines.empty())
            return;

        if (lines.size() == 1) {
            lines[0].clear();
            return;
        }

        if (y == 0) {
            Line& first = lines[0];
            Line& second = lines[1];

            second.insert(second.begin(),
                          first.begin(),
                          first.end());

            lines.erase(lines.begin());
            return;
        }

        Line& prev = lines[y - 1];
        Line& curr = lines[y];

        prev.insert(prev.end(),
                    curr.begin(),
                    curr.end());

        lines.erase(lines.begin() + y);
    }

    std::vector<std::string> Cake::getLines() const {
        std::vector<std::string> out;
        out.reserve(lines.size());

        for (size_t y = 0; y < lines.size(); ++y) {
            out.push_back(renderLine(y));
        }

        return out;
    }

    const std::string& Cake::getAddBuffer() const {
        return add;
    }

    const std::vector<Line>& Cake::getLinesRaw() const {
        return lines;
    }

    void Cake::setAddBuffer(const std::string& a) {
        add = a;
    }

    void Cake::setLinesRaw(const std::vector<Line>& l) {
        lines = l;
    }

}
