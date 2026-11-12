#pragma once

#include "data/Types.h"

// Fills an allocated maze with a random maze and 'doorCount' shortcut doors. Call srand() first.
void generateMaze(Maze& maze, int doorCount);

// Fewest steps from (0,0) to the exit (doors allowed), or -1 if there is no way.
int findShortestPathLength(const Maze& maze);