#include "MazeGenerator.h"

#include <algorithm>
#include <cstdlib>
#include <queue>

#include "Maze.h"

// "Recursive backtracker": rooms sit on even rows and columns. From the current room we go to a random
// unvisited room two tiles away and knock down the wall between. At a dead end we step back.
// The result has exactly one way between any two rooms.
static void carvePassages(Maze& maze) {
    vector<Position> stack;   // the rooms on our way; the last one is where we are now
    stack.push_back(makePosition(0, 0));
    setTile(maze, makePosition(0, 0), TileType::Path);

    while (!stack.empty()) {
        Position current = stack.back();

        vector<Direction> options;
        for (Direction direction : ALL_DIRECTIONS) {
            Position wall = getNeighbour(current, direction);
            Position room = getNeighbour(wall, direction);
            if (isInsideMaze(maze, room) && getTile(maze, room) == TileType::Wall) {
                options.push_back(direction);
            }
        }

        if (options.empty()) {
            stack.pop_back();
        }
        else {
            Direction direction = options[rand() % (int)options.size()];
            Position wall = getNeighbour(current, direction);
            Position room = getNeighbour(wall, direction);
            setTile(maze, wall, TileType::Path);
            setTile(maze, room, TileType::Path);
            stack.push_back(room);
        }
    }
}

// Even sizes have no room in the last row and column, so the exit needs its own opening.
static void openExit(Maze& maze) {
    int last = maze.size - 1;
    maze.tiles[last][last] = TileType::Path;
    if (last % 2 == 1) {
        maze.tiles[last - 1][last] = TileType::Path;
    }
}

static bool isOpen(const Maze& maze, int row, int col) {
    return maze.tiles[row][col] != TileType::Wall;
}

// A door replaces a wall that sits between two open tiles, so it makes a shortcut.
static void placeDoors(Maze& maze, int doorCount) {
    int placed = 0;
    for (int attempt = 0; attempt < 1000 && placed < doorCount; attempt++) {
        int row = 1 + rand() % (maze.size - 2);
        int col = 1 + rand() % (maze.size - 2);
        if (maze.tiles[row][col] != TileType::Wall) {
            continue;
        }
        bool betweenUpAndDown = isOpen(maze, row - 1, col) && isOpen(maze, row + 1, col);
        bool betweenLeftAndRight = isOpen(maze, row, col - 1) && isOpen(maze, row, col + 1);
        if (betweenUpAndDown || betweenLeftAndRight) {
            maze.tiles[row][col] = TileType::Door;
            placed++;
        }
    }
}

void generateMaze(Maze& maze, int doorCount) {
    carvePassages(maze);
    openExit(maze);
    placeDoors(maze, doorCount);
}

static bool canWalk(const Maze& maze, Position tile) {
    return isInsideMaze(maze, tile) && getTile(maze, tile) != TileType::Wall;
}

// Breadth-first search: visit the tiles ring by ring (1 step away, 2 steps away, ...).
// 'cameFrom' remembers from which tile every tile was reached, so at the end we can walk back from the goal.
vector<Position> findPath(const Maze& maze, Position from, Position to) {
    vector<vector<bool>> reached(maze.size, vector<bool>(maze.size, false));
    vector<vector<Position>> cameFrom(maze.size, vector<Position>(maze.size));
    queue<Position> toVisit;
    toVisit.push(from);
    reached[from.row][from.col] = true;

    while (!toVisit.empty()) {
        Position current = toVisit.front();
        toVisit.pop();
        for (Direction direction : ALL_DIRECTIONS) {
            Position next = getNeighbour(current, direction);
            if (canWalk(maze, next) && !reached[next.row][next.col]) {
                reached[next.row][next.col] = true;
                cameFrom[next.row][next.col] = current;
                toVisit.push(next);
            }
        }
    }

    vector<Position> path;
    if (!reached[to.row][to.col]) {
        return path;
    }
    Position step = to;
    path.push_back(step);
    while (!isSamePosition(step, from)) {
        step = cameFrom[step.row][step.col];
        path.push_back(step);
    }
    reverse(path.begin(), path.end());   // we walked from the goal back to the start
    return path;
}

// All tiles the snake can reach from the start without passing a gate (nearest first).
static vector<Position> findReachableTiles(const Maze& maze) {
    vector<vector<bool>> reached(maze.size, vector<bool>(maze.size, false));
    vector<Position> found;
    queue<Position> toVisit;
    toVisit.push(makePosition(0, 0));
    found.push_back(makePosition(0, 0));
    reached[0][0] = true;

    while (!toVisit.empty()) {
        Position current = toVisit.front();
        toVisit.pop();
        for (Direction direction : ALL_DIRECTIONS) {
            Position next = getNeighbour(current, direction);
            if (canWalk(maze, next) && getTile(maze, next) != TileType::Gate && !reached[next.row][next.col]) {
                reached[next.row][next.col] = true;
                found.push_back(next);
                toVisit.push(next);
            }
        }
    }
    return found;
}

static bool isInList(Position tile, const vector<Position>& list) {
    for (Position other : list) {
        if (isSamePosition(tile, other)) {
            return true;
        }
    }
    return false;
}

// The key goes as far from the start as possible (the list is ordered from near to far).
// First we try tiles off the way to the exit, so the snake has to make a detour for every key.
static Position pickKeyTile(const Maze& maze, const vector<Position>& reachable,
    const vector<Position>& wayOut, const vector<Position>& gates) {
    for (int i = (int)reachable.size() - 1; i > 0; i--) {
        if (getTile(maze, reachable[i]) == TileType::Path && !isInList(reachable[i], wayOut)) {
            return reachable[i];
        }
    }
    for (int i = (int)reachable.size() - 1; i > 0; i--) {
        if (getTile(maze, reachable[i]) == TileType::Path && !isInList(reachable[i], gates)) {
            return reachable[i];
        }
    }
    return reachable.back();
}

void placeKeysAndGates(GameState& state, int keyCount) {
    Maze& maze = state.maze;
    vector<Position> wayOut = findPath(maze, makePosition(0, 0), getExitPosition(maze));

    // The gates split the way to the exit into equal parts.
    vector<Position> gates;
    for (int i = 0; i < keyCount; i++) {
        gates.push_back(wayOut[(int)wayOut.size() * (i + 1) / (keyCount + 1)]);
    }

    // We start with the LAST gate. While it is closed (and the gates before it are not built yet),
    // every tile the snake can reach lies before that gate - a good place for its key.
    state.keys = vector<Position>(keyCount);
    for (int i = keyCount - 1; i >= 0; i--) {
        setTile(maze, gates[i], TileType::Gate);
        Position key = pickKeyTile(maze, findReachableTiles(maze), wayOut, gates);
        setTile(maze, key, TileType::Key);
        state.keys[i] = key;
    }
}
