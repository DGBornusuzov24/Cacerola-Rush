#include "MazeDisplay.h"

#include <algorithm>

#include "Maze.h"
#include "GameRules.h"
#include "PlayerMovement.h"
#include "Console.h"
#include "Panel.h"

const int SIDE_WIDTH = 30;   // width of the info panel next to the maze

// Wall colours from the top of the maze to the bottom, one palette per mode.
const int WALL_COLORS[MODE_COUNT][6] = {
    {39, 38, 33, 27, 63, 99},          // Classic: cyan -> violet
    {99, 135, 171, 207, 206, 205},     // Dark Maze: purple -> pink
    {226, 220, 214, 208, 202, 196},    // Move Limit: yellow -> red
    {48, 42, 36, 30, 24, 23},          // Key Hunt: green -> teal
    {196, 208, 226, 46, 51, 201}       // Chaos: rainbow
};
const int SNAKE_BODY_COLORS[SNAKE_MAX_TAIL] = { 40, 34, 34, 28, 28, 22 };   // the tail fades out

string getSnakeHeadGlyph() {
    return bgColor(46) + fgColor(16) + TEXT_BOLD + "oo" + resetColors();
}

string getWallGlyph(GameMode mode, int row, int mazeSize) {
    int shade = row * 6 / mazeSize;
    return fgColor(WALL_COLORS[(int)mode][shade]) + "██" + resetColors();
}

string getDoorGlyph() {
    return bgColor(234) + fgColor(THEME_CYAN) + "▒▒" + resetColors();
}

string getKeyGlyph() {
    return bgColor(234) + fgColor(THEME_GOLD) + TEXT_BOLD + "♦♦" + resetColors();
}

string getGateGlyph() {
    return bgColor(52) + fgColor(208) + TEXT_BOLD + "╫╫" + resetColors();
}

string getExitGlyph(bool bright) {
    int color = 208;
    if (bright) {
        color = THEME_GOLD;
    }
    return bgColor(color) + fgColor(16) + TEXT_BOLD + "EX" + resetColors();
}

string getTrailGlyph() {
    return bgColor(234) + fgColor(71) + " ·" + resetColors();
}

string getHintGlyph() {
    return bgColor(30) + fgColor(123) + " ·" + resetColors();
}

// A chessboard floor looks better than a flat one.
static string getFloorGlyph(int row, int col) {
    int color = 235;
    if ((row + col) % 2 == 0) {
        color = 234;
    }
    return bgColor(color) + "  " + resetColors();
}

static bool isOnHint(const vector<Position>& hint, Position tile) {
    for (int i = 0; i < (int)hint.size() && i < HINT_LENGTH; i++) {
        if (isSamePosition(hint[i], tile)) {
            return true;
        }
    }
    return false;
}

static string tileToGlyph(const GameState& state, const vector<Position>& hint, int row, int col) {
    if (!isTileVisible(state, row, col)) {
        return bgColor(232) + fgColor(236) + "░░" + resetColors();
    }
    Position tile = makePosition(row, col);
    if (isSamePosition(tile, state.player)) {
        return getSnakeHeadGlyph();
    }
    for (int i = 0; i < (int)state.tail.size(); i++) {
        if (isSamePosition(tile, state.tail[i])) {
            return bgColor(SNAKE_BODY_COLORS[i]) + "  " + resetColors();
        }
    }
    if (isSamePosition(tile, getExitPosition(state.maze))) {
        bool bright = getElapsedSeconds(state) % 2 == 0;   // the exit blinks
        return getExitGlyph(bright);
    }

    TileType type = getTile(state.maze, tile);
    if (type == TileType::Wall) {
        return getWallGlyph(state.mode, row, state.maze.size);
    }
    if (type == TileType::Door) {
        return getDoorGlyph();
    }
    if (type == TileType::Key) {
        return getKeyGlyph();
    }
    if (type == TileType::Gate) {
        return getGateGlyph();
    }
    if (isOnHint(hint, tile)) {
        return getHintGlyph();
    }
    if (state.maze.visited[row][col]) {
        return getTrailGlyph();
    }
    return getFloorGlyph(row, col);
}

static int keepBetween(int value, int low, int high) {
    if (value < low) {
        return low;
    }
    if (value > high) {
        return high;
    }
    return value;
}

static vector<string> buildMazeLines(const GameState& state, const vector<Position>& hint, int viewWidth, int viewHeight) {
    int size = state.maze.size;
    // the view follows the snake when the maze is bigger than the window
    int firstRow = keepBetween(state.player.row - viewHeight / 2, 0, size - viewHeight);
    int firstCol = keepBetween(state.player.col - viewWidth / 2, 0, size - viewWidth);

    string frame = fgColor(THEME_BORDER) + "▓▓" + resetColors();
    string frameLine = repeatText(frame, viewWidth + 2);

    vector<string> lines;
    lines.push_back(frameLine);
    for (int row = firstRow; row < firstRow + viewHeight; row++) {
        string line = frame;
        for (int col = firstCol; col < firstCol + viewWidth; col++) {
            line += tileToGlyph(state, hint, row, col);
        }
        lines.push_back(line + frame);
    }
    lines.push_back(frameLine);
    return lines;
}

static string infoRow(const string& label, const string& value, int color) {
    return fgColor(THEME_DIM) + padVisible(label, 8) + fgColor(color) + TEXT_BOLD + value + resetColors();
}

static string buildProgressBar(int percent) {
    string bar;
    for (int i = 0; i < 10; i++) {
        if (i < percent / 10) {
            bar += fgColor(THEME_GOOD) + "█";
        }
        else {
            bar += fgColor(239) + "░";
        }
    }
    return bar + fgColor(THEME_TEXT) + " " + to_string(percent) + "%";
}

static string buildKeyIcons(const GameState& state) {
    string icons;
    for (int i = 0; i < (int)state.keys.size(); i++) {
        if (i < state.keysCollected) {
            icons += fgColor(THEME_GOLD) + "♦ ";
        }
        else {
            icons += fgColor(240) + "♦ ";
        }
    }
    return icons + fgColor(THEME_DIM) + "(" + to_string(state.keysHeld) + " in hand)";
}

static string legendItem(const string& glyph, const string& name) {
    return padVisible(glyph + fgColor(THEME_TEXT) + " " + name, 14);
}

static vector<string> buildSidePanel(const GameState& state, int bestTime) {
    string separator = fgColor(60) + repeatText("─", SIDE_WIDTH - 2);
    string best = "--:--";
    if (bestTime >= 0) {
        best = formatTime(bestTime);
    }

    vector<string> lines;
    lines.push_back(fgColor(THEME_GOLD) + TEXT_BOLD + "S N A K E   M A Z E");
    lines.push_back(separator);
    lines.push_back(infoRow("MODE", getModeName(state.mode), THEME_TEXT));
    lines.push_back(infoRow("LEVEL", getDifficultyName(state.difficulty), getDifficultyColor(state.difficulty)));
    lines.push_back(separator);
    lines.push_back(infoRow("TIME", formatTime(getElapsedSeconds(state)), THEME_CYAN));
    lines.push_back(infoRow("BEST", best, THEME_GOLD));
    lines.push_back(infoRow("MOVES", to_string(state.moveCount), THEME_TEXT));
    if (state.moveLimit > 0) {
        int color = THEME_GOOD;
        if (getMovesLeft(state) <= 10) {
            color = THEME_BAD;
        }
        lines.push_back(infoRow("LEFT", to_string(getMovesLeft(state)), color));
    }
    if (!state.keys.empty()) {
        lines.push_back(infoRow("KEYS", buildKeyIcons(state), THEME_GOLD));
    }
    lines.push_back(infoRow("HINTS", to_string(getHintsLeft(state)) + " left", THEME_TEXT));
    lines.push_back(infoRow("EXIT", buildProgressBar(getProgressPercent(state)), THEME_GOOD));
    lines.push_back(separator);

    // the legend: two pictures per line
    vector<string> legend;
    legend.push_back(legendItem(getSnakeHeadGlyph(), "you"));
    legend.push_back(legendItem(getWallGlyph(state.mode, 0, 6), "wall"));
    if (!state.keys.empty()) {
        legend.push_back(legendItem(getKeyGlyph(), "key"));
        legend.push_back(legendItem(getGateGlyph(), "gate"));
    }
    else {
        legend.push_back(legendItem(getDoorGlyph(), "door"));
    }
    legend.push_back(legendItem(getExitGlyph(true), "exit"));
    legend.push_back(legendItem(getTrailGlyph(), "trail"));
    legend.push_back(legendItem(getHintGlyph(), "hint"));
    for (int i = 0; i < (int)legend.size(); i = i + 2) {
        string line = legend[i];
        if (i + 1 < (int)legend.size()) {
            line += legend[i + 1];
        }
        lines.push_back(line);
    }

    lines.push_back(separator);
    lines.push_back(fgColor(THEME_DIM) + "Arrows/WASD move    H hint");
    lines.push_back(fgColor(THEME_DIM) + "P pause             Q quit");
    if (areControlsReversed(state)) {
        lines.push_back(" ");
        lines.push_back(bgColor(THEME_BAD) + fgColor(231) + TEXT_BOLD + "  CONTROLS REVERSED!  ");
    }
    return lines;
}

void drawGame(const GameState& state, const string& message, int bestTime, const vector<Position>& hint) {
    int columns = 0;
    int rows = 0;
    getTerminalSize(columns, rows);

    // how many tiles fit next to the info panel (every tile is 2 characters wide)
    int viewWidth = keepBetween((columns - SIDE_WIDTH - 10) / 2, 5, state.maze.size);
    int viewHeight = keepBetween(rows - 5, 5, state.maze.size);

    vector<string> mazeLines = buildMazeLines(state, hint, viewWidth, viewHeight);
    vector<string> sideLines = buildSidePanel(state, bestTime);

    // the panel stands in the middle next to the maze
    int panelTop = max(0, ((int)mazeLines.size() - (int)sideLines.size()) / 2);
    int height = max((int)mazeLines.size(), panelTop + (int)sideLines.size());
    string emptyMazeLine((viewWidth + 2) * 2, ' ');

    vector<string> lines;
    for (int i = 0; i < height; i++) {
        string left = emptyMazeLine;
        if (i < (int)mazeLines.size()) {
            left = mazeLines[i];
        }
        string right;
        int sideIndex = i - panelTop;
        if (sideIndex >= 0 && sideIndex < (int)sideLines.size()) {
            right = sideLines[sideIndex] + resetColors();
        }
        lines.push_back(left + "    " + padVisible(right, SIDE_WIDTH));
    }
    lines.push_back(" ");
    lines.push_back(fgColor(THEME_GOLD) + TEXT_BOLD + message + " ");
    drawCentered(lines);
}
