#include "logic/MazeGenerator.h"

#include <cstdlib>
#include "data/Maze.h"

namespace {

    const int ROW_STEP[4] = { -1, 1, 0, 0 };   // up, down, left, right
    const int COL_STEP[4] = { 0, 0, -1, 1 };

    // "Recursive backtracker": rooms sit on even rows and columns. We walk to a random unvisited room and
    // knock down the wall between, and step back at dead ends. This gives exactly one route between any two rooms.
    void carvePassages(Maze& maze) {
        int rooms = ((maze.size + 1) / 2) * ((maze.size + 1) / 2);
        int* stackRow = new int[rooms];
        int* stackCol = new int[rooms];
        int stackSize = 1;
        stackRow[0] = 0;
        stackCol[0] = 0;
        maze.tiles[0][0] = TileType::Path;

        while (stackSize > 0) {
            int row = stackRow[stackSize - 1];
            int col = stackCol[stackSize - 1];

            int options[4];
            int optionCount = 0;
            for (int d = 0; d < 4; d++) {
                int nextRow = row + 2 * ROW_STEP[d];
                int nextCol = col + 2 * COL_STEP[d];
                if (isInsideMaze(maze, nextRow, nextCol) && maze.tiles[nextRow][nextCol] == TileType::Wall) {
                    options[optionCount++] = d;
                }
            }
            if (optionCount == 0) {
                stackSize--;   // dead end: go back
                continue;
            }

            int d = options[rand() % optionCount];
            stackRow[stackSize] = row + 2 * ROW_STEP[d];
            stackCol[stackSize] = col + 2 * COL_STEP[d];
            maze.tiles[row + ROW_STEP[d]][col + COL_STEP[d]] = TileType::Path;
            maze.tiles[stackRow[stackSize]][stackCol[stackSize]] = TileType::Path;
            stackSize++;
        }
        delete[] stackRow;
        delete[] stackCol;
    }

    // Even sizes have no room in the last row / column, so the exit needs its own link.
    void openExit(Maze& maze) {
        int last = maze.size - 1;
        maze.tiles[last][last] = TileType::Path;
        if (last % 2 == 1) {
            maze.tiles[last - 1][last] = TileType::Path;
        }
    }

    void placeDoors(Maze& maze, int doorCount) {
        for (int placed = 0, attempts = 0; placed < doorCount && attempts < doorCount * 100; attempts++) {
            int row = 1 + rand() % (maze.size - 2);
            int col = 1 + rand() % (maze.size - 2);
            if (maze.tiles[row][col] != TileType::Wall) {
                continue;
            }
            // A door replaces a wall that separates two open tiles (above/below or left/right).
            bool betweenRows = row % 2 == 1 && col % 2 == 0 &&
                maze.tiles[row - 1][col] != TileType::Wall && maze.tiles[row + 1][col] != TileType::Wall;
            bool betweenCols = row % 2 == 0 && col % 2 == 1 &&
                maze.tiles[row][col - 1] != TileType::Wall && maze.tiles[row][col + 1] != TileType::Wall;
            if (betweenRows || betweenCols) {
                maze.tiles[row][col] = TileType::Door;
                placed++;
            }
        }
    }

}  // namespace

void generateMaze(Maze& maze, int doorCount) {
    carvePassages(maze);
    openExit(maze);
    placeDoors(maze, doorCount);
}

// Breadth-first search: visit the tiles ring by ring (1 step away, 2 steps away, ...) until the exit is reached.
int findShortestPathLength(const Maze& maze) {
    int total = maze.size * maze.size;
    int* steps = new int[total];       // steps needed to reach each tile, -1 = not reached yet
    int* waiting = new int[total];     // queue of tiles to visit
    for (int i = 0; i < total; i++) {
        steps[i] = -1;
    }
    int queueStart = 0;
    int queueEnd = 1;
    waiting[0] = 0;
    steps[0] = 0;

    while (queueStart < queueEnd) {
        int current = waiting[queueStart++];
        for (int d = 0; d < 4; d++) {
            int nextRow = current / maze.size + ROW_STEP[d];
            int nextCol = current % maze.size + COL_STEP[d];
            int next = nextRow * maze.size + nextCol;
            if (isInsideMaze(maze, nextRow, nextCol) && maze.tiles[nextRow][nextCol] != TileType::Wall && steps[next] == -1) {
                steps[next] = steps[current] + 1;
                waiting[queueEnd++] = next;
            }
        }
    }
    int result = steps[total - 1];
    delete[] steps;
    delete[] waiting;
    return result;
}