#pragma once

#include "data/Types.h"

void allocateMaze(Maze& maze, int size);   // every tile starts as a wall
void freeMaze(Maze& maze);
bool isInsideMaze(const Maze& maze, int row, int col);
