#include "GameRules.h"

#include "Maze.h"
#include "MazeGenerator.h"

// What changes with the level.
struct LevelSettings {
    int doorDivisor;    // doors = maze size / doorDivisor
    int fogRadius;      // how far the snake sees in the fog
    int limitPercent;   // move limit = best route * limitPercent / 100 + limitExtra
    int limitExtra;
    int keyCount;
};

const LevelSettings LEVELS[4] = {
    {3, 6, 200, 15, 1},    // Easy
    {5, 4, 150, 10, 2},    // Normal
    {10, 2, 120, 5, 3},    // Hard
    {10, 3, 140, 10, 3}    // Full Chaos
};

bool isValidMazeSize(int size) {
    return size >= MIN_MAZE_SIZE && size <= MAX_MAZE_SIZE;
}

bool modeHasFog(GameMode mode) {
    return mode == GameMode::Fog || mode == GameMode::Chaos;
}

bool modeHasMoveLimit(GameMode mode) {
    return mode == GameMode::MoveLimit || mode == GameMode::Chaos;
}

bool modeHasKeys(GameMode mode) {
    return mode == GameMode::Keys || mode == GameMode::Chaos;
}

static int countSteps(const Maze& maze, Position from, Position to) {
    return (int)findPath(maze, from, to).size() - 1;
}

// The fewest moves to win: start -> every key in order -> exit.
static int findBestRoute(const GameState& state) {
    int total = 0;
    Position from = state.player;
    for (Position key : state.keys) {
        total = total + countSteps(state.maze, from, key);
        from = key;
    }
    return total + countSteps(state.maze, from, getExitPosition(state.maze));
}

void startNewGame(GameState& state, int size, GameMode mode, Difficulty difficulty) {
    endGame(state);         // free the old maze, if there is one
    state = GameState();    // every counter back to zero
    state.mode = mode;
    state.difficulty = difficulty;
    LevelSettings level = LEVELS[(int)difficulty];

    allocateMaze(state.maze, size);
    if (modeHasKeys(mode)) {
        generateMaze(state.maze, 0);   // no doors: they could lead around the gates
        placeKeysAndGates(state, level.keyCount);
    }
    else {
        generateMaze(state.maze, size / level.doorDivisor);
    }
    state.maze.visited[0][0] = true;

    state.bestRoute = findBestRoute(state);
    state.exitDistanceAtStart = countSteps(state.maze, state.player, getExitPosition(state.maze));
    if (modeHasMoveLimit(mode)) {
        state.moveLimit = state.bestRoute * level.limitPercent / 100 + level.limitExtra;
    }
    state.startTime = time(nullptr);
}

void endGame(GameState& state) {
    freeMaze(state.maze);
}

void restartTimer(GameState& state) {
    state.startTime = time(nullptr);
}

void continueAfterPause(GameState& state, int pausedSeconds) {
    state.startTime = state.startTime + pausedSeconds;
}

int getElapsedSeconds(const GameState& state) {
    int seconds = (int)(time(nullptr) - state.startTime);
    return seconds + state.hintsUsed * HINT_PENALTY_SECONDS;
}

bool hasPlayerWon(const GameState& state) {
    return isSamePosition(state.player, getExitPosition(state.maze));
}

bool hasPlayerLost(const GameState& state) {
    if (state.moveLimit == 0 || hasPlayerWon(state)) {
        return false;
    }
    return state.moveCount >= state.moveLimit;
}

int getMovesLeft(const GameState& state) {
    return state.moveLimit - state.moveCount;
}

bool isTileVisible(const GameState& state, int row, int col) {
    if (!modeHasFog(state.mode)) {
        return true;
    }
    bool isExit = isSamePosition(makePosition(row, col), getExitPosition(state.maze));
    bool isKey = state.maze.tiles[row][col] == TileType::Key;
    if (isExit || isKey || state.maze.visited[row][col]) {
        return true;
    }
    int radius = LEVELS[(int)state.difficulty].fogRadius;
    int rowDistance = row - state.player.row;
    int colDistance = col - state.player.col;
    return rowDistance * rowDistance + colDistance * colDistance <= radius * radius;
}

int getHintsLeft(const GameState& state) {
    return MAX_HINTS - state.hintsUsed;
}

vector<Position> useHint(GameState& state) {
    if (getHintsLeft(state) <= 0) {
        return vector<Position>();
    }
    state.hintsUsed++;

    // The next goal is the first key still lying in the maze, otherwise the exit.
    Position goal = getExitPosition(state.maze);
    for (Position key : state.keys) {
        if (getTile(state.maze, key) == TileType::Key) {
            goal = key;
            break;
        }
    }
    return findPath(state.maze, state.player, goal);
}

int getProgressPercent(const GameState& state) {
    int distance = countSteps(state.maze, state.player, getExitPosition(state.maze));
    int percent = 100 - distance * 100 / state.exitDistanceAtStart;
    if (percent < 0) {
        percent = 0;
    }
    return percent;
}

int getStarRating(const GameState& state) {
    if (state.moveCount <= state.bestRoute * 12 / 10) {
        return 3;
    }
    if (state.moveCount <= state.bestRoute * 2) {
        return 2;
    }
    return 1;
}
