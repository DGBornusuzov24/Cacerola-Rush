#include "Maze.h"

void allocateMaze(Maze& maze, int size) {
    freeMaze(maze);
    maze.size = size;
    maze.tiles = new TileType * [size];
    maze.visited = new bool* [size];

    for (int row = 0; row < size; row++) {
        maze.tiles[row] = new TileType[size];
        maze.visited[row] = new bool[size];
        for (int col = 0; col < size; col++) {
            maze.tiles[row][col] = TileType::Wall;
            maze.visited[row][col] = false;
        }
    }
}

void freeMaze(Maze& maze) {
    if (maze.tiles == nullptr) {
        return;
    }
    for (int row = 0; row < maze.size; row++) {
        delete[] maze.tiles[row];
        delete[] maze.visited[row];
    }
    delete[] maze.tiles;
    delete[] maze.visited;
    maze.tiles = nullptr;
    maze.visited = nullptr;
    maze.size = 0;
}

bool isInsideMaze(const Maze& maze, Position tile) {
    return tile.row >= 0 && tile.row < maze.size && tile.col >= 0 && tile.col < maze.size;
}

TileType getTile(const Maze& maze, Position tile) {
    return maze.tiles[tile.row][tile.col];
}

void setTile(Maze& maze, Position tile, TileType type) {
    maze.tiles[tile.row][tile.col] = type;
}

Position getExitPosition(const Maze& maze) {
    return makePosition(maze.size - 1, maze.size - 1);
}

Position makePosition(int row, int col) {
    Position position;
    position.row = row;
    position.col = col;
    return position;
}

bool isSamePosition(Position first, Position second) {
    return first.row == second.row && first.col == second.col;
}

Position getNeighbour(Position tile, Direction direction) {
    if (direction == Direction::Up) {
        tile.row--;
    }
    else if (direction == Direction::Down) {
        tile.row++;
    }
    else if (direction == Direction::Left) {
        tile.col--;
    }
    else {
        tile.col++;
    }
    return tile;
}
