#include "presentation/MazeDisplay.h"

#include <algorithm>

#include "logic/GameRules.h"
#include "presentation/Console.h"
#include "presentation/Panel.h"

namespace {

    const int PALETTE_SIZE = 6;
    const int WALL_PALETTES[3][PALETTE_SIZE] = {   // one palette per mode, from the top of the maze to the bottom
        { 39, 38, 33, 27, 63, 99 },
        { 99, 135, 171, 207, 206, 205 },
        { 226, 220, 214, 208, 202, 196 }
    };
    const int SNAKE_BODY_COLORS[SNAKE_MAX_TAIL] = { 40, 34, 34, 28, 28, 22 };

    string getFloorGlyph(int row, int col) {
        return bgColor((row + col) % 2 == 0 ? 233 : 235) + "  " + resetColors();   // chessboard floor
    }

    string tileToGlyph(const GameState& state, int row, int col) {
        if (!isTileVisible(state, row, col)) {
            return bgColor(232) + fgColor(236) + "░░" + resetColors();
        }
        if (row == state.player.row && col == state.player.col) {
            return getSnakeHeadGlyph();
        }
        for (int i = 0; i < state.tailLength; i++) {
            if (row == state.tail[i].row && col == state.tail[i].col) {
                return bgColor(SNAKE_BODY_COLORS[i]) + "  " + resetColors();
            }
        }
        if (row == state.maze.size - 1 && col == state.maze.size - 1) {
            return getExitGlyph(getElapsedSeconds(state) % 2 == 0);   // the exit blinks
        }
        switch (state.maze.tiles[row][col]) {
        case TileType::Wall: return getWallGlyph(state.mode, row, state.maze.size);
        case TileType::Door: return getDoorGlyph();
        default:             return getFloorGlyph(row, col);
        }
    }

    string stat(const string& label, const string& value, int color = THEME_GOLD) {
        return fgColor(THEME_DIM) + label + " " + fgColor(color) + TEXT_BOLD + value + resetColors() + "    ";
    }

    string buildHud(const GameState& state, int bestTime) {
        string hud = stat("TIME", formatTime(getElapsedSeconds(state)), 51);
        hud += stat("BEST", bestTime < 0 ? "--:--" : formatTime(bestTime));
        hud += stat("MOVES", to_string(state.moveCount));
        if (state.mode == GameMode::MoveLimit) {
            hud += stat("LEFT", to_string(getMovesLeft(state)), getMovesLeft(state) <= 10 ? THEME_BAD : THEME_GOOD);
        }
        return hud + stat("POS", to_string(state.player.row) + "," + to_string(state.player.col));
    }

}  // namespace

string getSnakeHeadGlyph() {
    return bgColor(46) + fgColor(16) + TEXT_BOLD + "oo" + resetColors();
}

string getWallGlyph(GameMode mode, int row, int mazeSize) {
    return fgColor(WALL_PALETTES[static_cast<int>(mode)][row * PALETTE_SIZE / mazeSize]) + "██" + resetColors();
}

string getDoorGlyph() {
    return bgColor(233) + fgColor(51) + "▒▒" + resetColors();
}

string getExitGlyph(bool bright) {
    return bgColor(bright ? THEME_GOLD : 208) + fgColor(16) + TEXT_BOLD + "EX" + resetColors();
}

void drawGame(const GameState& state, const string& message, int bestTime) {
    int size = state.maze.size;
    int columns = 0;
    int rows = 0;
    getTerminalSize(columns, rows);

    // 2 characters per tile, and some rows are needed for the texts. The view follows the snake.
    int viewWidth = min(size, max(5, (columns - 8) / 2 - 2));
    int viewHeight = min(size, max(5, rows - 10));
    int firstCol = clamp(state.player.col - viewWidth / 2, 0, size - viewWidth);
    int firstRow = clamp(state.player.row - viewHeight / 2, 0, size - viewHeight);

    string frame = fgColor(THEME_BORDER) + "▓▓" + resetColors();
    string frameLine;
    for (int i = 0; i < viewWidth + 2; i++) {
        frameLine += frame;
    }

    vector<string> lines;
    lines.push_back(fgColor(THEME_GOLD) + TEXT_BOLD + "══  S N A K E   M A Z E  ══   " + resetColors() +
        fgColor(THEME_TEXT) + getModeName(state.mode) + " · " +
        fgColor(getDifficultyColor(state.difficulty)) + getDifficultyName(state.difficulty));
    lines.push_back(" ");
    lines.push_back(frameLine);
    for (int row = firstRow; row < firstRow + viewHeight; row++) {
        string line = frame;
        for (int col = firstCol; col < firstCol + viewWidth; col++) {
            line += tileToGlyph(state, row, col);
        }
        lines.push_back(line + frame);
    }
    lines.push_back(frameLine);
    lines.push_back(" ");
    lines.push_back(buildHud(state, bestTime));
    lines.push_back(fgColor(THEME_DIM) + "Arrows / W A S D move      Q back to menu");
    lines.push_back(message.empty() ? " " : fgColor(THEME_GOLD) + message);
    drawCentered(lines);
}