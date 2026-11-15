#include "Panel.h"

#include "InputHandler.h"

// Every letter of the title is 5 rows of 5 "pixels" ('#' = filled).
const string TITLE_PIXELS[5][4] = {
    {"#   #", " ### ", "#####", "#####"},
    {"## ##", "#   #", "   # ", "#    "},
    {"# # #", "#####", "  #  ", "#### "},
    {"#   #", "#   #", " #   ", "#    "},
    {"#   #", "#   #", "#####", "#####"}
};
const int TITLE_COLORS[5] = { 213, 177, 141, 105, 69 };

string getModeName(GameMode mode) {
    const string names[MODE_COUNT] = { "Classic", "Dark Maze", "Move Limit", "Key Hunt", "Chaos" };
    return names[(int)mode];
}

string getDifficultyName(Difficulty difficulty) {
    const string names[4] = { "Easy", "Normal", "Hard", "Full Chaos" };
    return names[(int)difficulty];
}

int getDifficultyColor(Difficulty difficulty) {
    const int colors[4] = { THEME_GOOD, THEME_GOLD, THEME_BAD, THEME_SELECTED };
    return colors[(int)difficulty];
}

string formatTime(int seconds) {
    string minutes = to_string(seconds / 60);
    string rest = to_string(seconds % 60);
    if (minutes.size() < 2) {
        minutes = "0" + minutes;
    }
    if (rest.size() < 2) {
        rest = "0" + rest;
    }
    return minutes + ":" + rest;
}

string buildSnakeBanner(int background) {
    const int shades[9] = { 22, 22, 28, 28, 34, 34, 40, 40, 46 };
    string snake;
    for (int i = 0; i < 9; i++) {
        snake += bgColor(shades[i]) + "  ";
    }
    snake += bgColor(46) + fgColor(16) + "oo";                 // the head with two eyes
    snake += bgColor(background) + fgColor(196) + " »";       // the tongue
    return snake;
}

vector<string> buildTitle() {
    vector<string> lines;
    for (int row = 0; row < 5; row++) {
        string line;
        for (int letter = 0; letter < 4; letter++) {
            for (char pixel : TITLE_PIXELS[row][letter]) {
                if (pixel == '#') {
                    line += "██";
                }
                else {
                    line += "  ";
                }
            }
            line += "  ";
        }
        lines.push_back(fgColor(TITLE_COLORS[row]) + line);
    }
    lines.push_back(" ");
    lines.push_back(buildSnakeBanner(THEME_BACKGROUND));
    lines.push_back(" ");
    return lines;
}

static string borderStyle() {
    return fgColor(THEME_BORDER) + bgColor(THEME_PANEL);
}

string textStyle() {
    return fgColor(THEME_TEXT) + bgColor(THEME_PANEL);
}

string boxTop() {
    return borderStyle() + "╔" + repeatText("═", BOX_WIDTH) + "╗";
}

string boxSeparator() {
    return borderStyle() + "╠" + repeatText("═", BOX_WIDTH) + "╣";
}

string boxBottom() {
    return borderStyle() + "╚" + repeatText("═", BOX_WIDTH) + "╝";
}

// 'content' may have colours; the rest of the row is filled with spaces.
string boxLine(const string& content, int background) {
    return borderStyle() + "║" + bgColor(background) + padVisible(content, BOX_WIDTH) + borderStyle() + "║";
}

string boxTitle(const string& heading) {
    int left = (BOX_WIDTH - (int)heading.size()) / 2;
    return boxLine(string(left, ' ') + fgColor(THEME_GOLD) + TEXT_BOLD + heading);
}

string boxText(const string& text) {
    return boxLine(textStyle() + "  " + text);
}

string boxGlyphText(const string& glyph, const string& text) {
    return boxLine("  " + glyph + textStyle() + "  " + text);
}

void showBox(const vector<string>& rows) {
    vector<string> lines;
    lines.push_back(boxTop());
    for (const string& row : rows) {
        lines.push_back(row);
    }
    lines.push_back(boxBottom());
    lines.push_back(" ");
    lines.push_back(fgColor(THEME_DIM) + "Press any key to continue");
    drawCentered(lines);
    readKey();
}
