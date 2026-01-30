#ifndef DRAW_H
#define DRAW_H

#include <vector>
#include <string>
#include "terminal.h"

namespace tui {

static constexpr const char* TL = "┌";
static constexpr const char* TR = "┐";
static constexpr const char* BL = "└";
static constexpr const char* BR = "┘";
static constexpr const char* H  = "─";
static constexpr const char* V  = "│";

void drawLine(const std::string& line, const std::vector<size_t>& matches, size_t len);

void moveCursor(int r, int c);

void drawBox(int x, int y, int w, int h);

void drawCenteredBox(const Window& window, int w, int h, int& outX, int& outY);

}

#endif // DRAW_H