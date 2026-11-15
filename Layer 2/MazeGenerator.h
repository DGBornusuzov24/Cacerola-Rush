#pragma once

#include <vector>
#include "Types.h"

// Fills an allocated maze with a random maze and 'doorCount' secret doors. Call srand() once before.
void generateMaze(Maze& maze, int doorCount);

// Puts 'keyCount' locked gates on the way to the exit and one key before each gate.
// Use it on a maze without doors (doors could lead around the gates).
void placeKeysAndGates(GameState& state, int keyCount);

// The shortest way from 'from' to 'to', both included. Only walls block it. Empty if there is no way.
vector<Position> findPath(const Maze& maze, Position from, Position to);
