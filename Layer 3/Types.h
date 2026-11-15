#pragma once

#include <ctime>
#include <string>
#include <vector>
using namespace std;

enum class TileType { Wall, Path, Door, Key, Gate };
enum class Direction { Up, Down, Left, Right };
enum class GameMode { Classic, Fog, MoveLimit, Keys, Chaos };
enum class Difficulty { Easy, Normal, Hard, FullChaos };

const int MODE_COUNT = 5;
const int SNAKE_MAX_TAIL = 6;
const Direction ALL_DIRECTIONS[4] = { Direction::Up, Direction::Down, Direction::Left, Direction::Right };

struct Position {
    int row = 0;
    int col = 0;
};

// The maze uses two dynamic 2D arrays of the same size: [row][col].
struct Maze {
    int size = 0;
    TileType** tiles = nullptr;
    bool** visited = nullptr;   // true where the snake has already been (its trail)
};

struct GameState {
    Maze maze;
    GameMode mode = GameMode::Classic;
    Difficulty difficulty = Difficulty::Normal;

    Position player;          // the snake's head
    vector<Position> tail;    // tail[0] is right behind the head
    int moveCount = 0;
    int moveLimit = 0;        // 0 = no limit

    vector<Position> keys;    // where the keys lie (Key Hunt and Chaos), in the order of the gates
    int keysCollected = 0;
    int keysHeld = 0;         // collected keys that were not used on a gate yet

    int bestRoute = 0;        // the fewest moves needed to win
    int exitDistanceAtStart = 0;
    int hintsUsed = 0;
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
