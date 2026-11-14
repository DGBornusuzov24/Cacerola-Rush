#pragma once
#include "data/Types.h"

const int MIN_MAZE_SIZE = 10;
const int MAX_MAZE_SIZE = 50;

bool isValidMazeSize(int size);

// Builds a new random maze for the chosen mode and difficulty and starts the timer.
void startNewGame(GameState& state, int size, GameMode mode, Difficulty difficulty);
void endGame(GameState& state);

bool hasPlayerWon(const GameState& state);
bool hasPlayerLost(const GameState& state);   // Move Limit mode: no moves left
int getMovesLeft(const GameState& state);
int getElapsedSeconds(const GameState& state);

// Always true, except in Dark Maze mode (the exit is always visible).
bool isTileVisible(const GameState& state, int row, int col);
