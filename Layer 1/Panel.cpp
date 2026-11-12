#include "presentation/Panel.h"

#include <algorithm>
#include <cstdio>

#include "presentation/Console.h"
#include "presentation/InputHandler.h"

namespace {

    // Every letter of the title: 5 rows of 5 "pixels" ('#' = filled)
    const char* const TITLE_PIXELS[5][4] = {
        { "#   #", " ### ", "#####", "#####" },
        { "## ##", "#   #", "   # ", "#    " },
        { "# # #", "#####", "  #  ", "#### " },
        { "#   #", "#   #", " #   ", "#    " },
        { "#   #", "#   #", "#####", "#####" }
    };
    const int TITLE_COLORS[5] = { 213, 177, 141, 105, 69 };

    string borderStyle() {
        return fgColor(THEME_BORDER) + bgColor(THEME_PANEL);
    }

    string repeatText(const string& text, int times) {
        string result;
        for (int i = 0; i < times; i++) {
            result += text;
        }
        return result;
    }

}  // namespace

string getModeName(GameMode mode) {
    const char* names[] = { "Classic", "Dark Maze", "Move Limit" };
    return names[static_cast<int>(mode)];
}

string getDifficultyName(Difficulty difficulty) {
    const char* names[] = { "Easy", "Normal", "Hard" };
    return names[static_cast<int>(difficulty)];
}

int getDifficultyColor(Difficulty difficulty) {
    const int colors[] = { THEME_GOOD, THEME_GOLD, THEME_BAD };
    return colors[static_cast<int>(difficulty)];
}

string formatTime(int seconds) {
    char text[16];
    snprintf(text, sizeof(text), "%02d:%02d", seconds / 60, seconds % 60);
    return text;
}

string padRight(const string& text, int width) {
    return text + string(max(0, width - static_cast<int>(text.size())), ' ');
}

string buildSnakeBanner(int background) {
    const int shades[] = { 22, 22, 28, 28, 34, 34, 40, 40, 46 };
    string snake;
    for (int shade : shades) {
        snake += bgColor(shade) + "  ";
    }
    return snake + bgColor(46) + fgColor(16) + "oo" + bgColor(background) + fgColor(196) + " »";
}

vector<string> buildTitle() {
    vector<string> lines;
    for (int row = 0; row < 5; row++) {
        string line;
        for (int letter = 0; letter < 4; letter++) {
            for (const char* pixel = TITLE_PIXELS[row][letter]; *pixel != '\0'; pixel++) {
                line += (*pixel == '#') ? "██" : "  ";
            }
            line += "  ";
        }
        lines.push_back(fgColor(TITLE_COLORS[row]) + line);
    }
    lines.insert(lines.end(), { " ", buildSnakeBanner(), " " });
    return lines;
}

string textStyle() {
    return fgColor(THEME_TEXT) + bgColor(THEME_PANEL);
}

string panelTop() {
    return borderStyle() + "╔" + repeatText("═", PANEL_WIDTH) + "╗";
}

string panelSeparator() {
    return borderStyle() + "╠" + repeatText("═", PANEL_WIDTH) + "╣";
}

string panelBottom() {
    return borderStyle() + "╚" + repeatText("═", PANEL_WIDTH) + "╝";
}

// The content may have colours; the rest of the row is filled with spaces.
string panelLine(const string& content, int background) {
    int padding = max(0, PANEL_WIDTH - visibleLength(content));
    return borderStyle() + "║" + bgColor(background) + content + string(padding, ' ') + borderStyle() + "║";
}

string panelTitle(const string& heading) {
    int left = (PANEL_WIDTH - static_cast<int>(heading.size())) / 2;
    return panelLine(string(left, ' ') + fgColor(THEME_GOLD) + TEXT_BOLD + heading);
}

string panelText(const string& text) {
    return panelLine(textStyle() + "  " + text);
}

string panelGlyphText(const string& glyph, const string& text) {
    return panelLine("  " + glyph + textStyle() + "  " + text);
}

void showBox(const vector<string>& rows) {
    vector<string> lines = { panelTop() };
    lines.insert(lines.end(), rows.begin(), rows.end());
    lines.insert(lines.end(), { panelBottom(), " ", fgColor(THEME_DIM) + "Press any key to continue" });
    drawCentered(lines);
    readKey();
}
