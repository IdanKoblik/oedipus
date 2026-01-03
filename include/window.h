#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <vector>

namespace window {

    static constexpr const char* TL = "┌";
    static constexpr const char* TR = "┐";
    static constexpr const char* BL = "└";
    static constexpr const char* BR = "┘";
    static constexpr const char* H  = "─";
    static constexpr const char* V  = "│";

    void getWindowSize(int &rows, int &cols);

    void drawLine(const std::string& line, const std::vector<size_t>& matches, size_t len);

    void writeStr(const std::string &s);
    void moveCursor(int r, int c);

    void drawBox(int x, int y, int w, int h);
    void drawCenteredBox(int w, int h, int& outX, int& outY);

    void clear();
}

#endif //WINDOW_H