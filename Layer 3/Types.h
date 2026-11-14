#pragma once

#include <ctime>
#include <string>
using namespace std;

enum class TileType { Wall, Path, Door };
enum class Direction { Up, Down, Left, Right };
enum class GameMode { Classic, Fog, MoveLimit };
enum class Difficulty { Easy, Normal, Hard };

const int SNAKE_MAX_TAIL = 6;

struct Position {
    int row = 0;
    int col = 0;
};

// 'tiles' is a dynamic 2D array: tiles[row][col]
struct Maze {
    int size = 0;
    TileType** tiles = nullptr;

    Maze() = default;
    Maze(const Maze&) = delete;   // copying would share the same memory
    Maze& operator=(const Maze&) = delete;
};

struct GameState {
    Maze maze;
    Position player;
    Position tail[SNAKE_MAX_TAIL];   // tail[0] is right behind the head
    int tailLength = 0;
    int moveCount = 0;
    int moveLimit = 0;               // only used in Move Limit mode
    GameMode mode = GameMode::Classic;
    Difficulty difficulty = Difficulty::Normal;
    time_t startTime = 0;
};

// One finished game in the leaderboard.
struct ScoreEntry {
    string name;
    GameMode mode = GameMode::Classic;
    Difficulty difficulty = Difficulty::Normal;
    int size = 0;
    int seconds = 0;
    int moves = 0;
    int score = 0;
};
