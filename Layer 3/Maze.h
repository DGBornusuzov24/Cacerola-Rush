#pragma once

#include "Types.h"

void allocateMaze(Maze& maze, int size);   // every tile starts as a wall, nothing is visited
void freeMaze(Maze& maze);

bool isInsideMaze(const Maze& maze, Position tile);
TileType getTile(const Maze& maze, Position tile);
void setTile(Maze& maze, Position tile, TileType type);
Position getExitPosition(const Maze& maze);   // the bottom-right corner

Position makePosition(int row, int col);
bool isSamePosition(Position first, Position second);
Position getNeighbour(Position tile, Direction direction);   // the tile one step away
