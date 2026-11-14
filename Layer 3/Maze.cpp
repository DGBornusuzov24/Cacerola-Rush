#include "data/Maze.h"

void allocateMaze(Maze& maze, int size) {
    freeMaze(maze);
    maze.size = size;
    maze.tiles = new TileType * [size];
    for (int row = 0; row < size; row++) {
        maze.tiles[row] = new TileType[size];
        for (int col = 0; col < size; col++) {
            maze.tiles[row][col] = TileType::Wall;
        }
    }
}

void freeMaze(Maze& maze) {
    if (maze.tiles == nullptr) {
        return;
    }
    for (int row = 0; row < maze.size; row++) {
        delete[] maze.tiles[row];
    }
    delete[] maze.tiles;
    maze.tiles = nullptr;
    maze.size = 0;
}

bool isInsideMaze(const Maze& maze, int row, int col) {
    return row >= 0 && row < maze.size && col >= 0 && col < maze.size;
}
