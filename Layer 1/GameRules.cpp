#include "logic/GameRules.h"

#include "data/Maze.h"
#include "logic/MazeGenerator.h"

namespace {

    // What changes with the difficulty: doors = size / doorDivisor, how far the snake sees in the fog,
    // and the Move Limit = shortest route * limitPercent / 100 + limitExtra.
    struct DifficultySettings {
        int doorDivisor;
        int fogRadius;
        int limitPercent;
        int limitExtra;
    };

    const DifficultySettings SETTINGS[3] = {
        { 3, 6, 200, 15 },    // Easy
        { 5, 4, 150, 10 },    // Normal
        { 10, 2, 120, 5 }     // Hard
    };

    const DifficultySettings& getSettings(const GameState& state) {
        return SETTINGS[static_cast<int>(state.difficulty)];
    }

}  // namespace

bool isValidMazeSize(int size) {
    return size >= MIN_MAZE_SIZE && size <= MAX_MAZE_SIZE;
}

void startNewGame(GameState& state, int size, GameMode mode, Difficulty difficulty) {
    state.mode = mode;
    state.difficulty = difficulty;
    allocateMaze(state.maze, size);
    generateMaze(state.maze, size / getSettings(state).doorDivisor);

    state.player = Position();
    state.tailLength = 0;
    state.moveCount = 0;
    state.moveLimit = 0;
    if (mode == GameMode::MoveLimit) {
        state.moveLimit = findShortestPathLength(state.maze) * getSettings(state).limitPercent / 100 + getSettings(state).limitExtra;
    }
    state.startTime = time(nullptr);
}

void endGame(GameState& state) {
    freeMaze(state.maze);
}

bool hasPlayerWon(const GameState& state) {
    return state.player.row == state.maze.size - 1 && state.player.col == state.maze.size - 1;
}

bool hasPlayerLost(const GameState& state) {
    return state.mode == GameMode::MoveLimit && state.moveCount >= state.moveLimit && !hasPlayerWon(state);
}

int getMovesLeft(const GameState& state) {
    return state.moveLimit - state.moveCount;
}

int getElapsedSeconds(const GameState& state) {
    return static_cast<int>(time(nullptr) - state.startTime);
}

bool isTileVisible(const GameState& state, int row, int col) {
    if (state.mode != GameMode::Fog || (row == state.maze.size - 1 && col == state.maze.size - 1)) {
        return true;
    }
    int radius = getSettings(state).fogRadius;
    int rowDistance = row - state.player.row;
    int colDistance = col - state.player.col;
    return rowDistance * rowDistance + colDistance * colDistance <= radius * radius;
}
