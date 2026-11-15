#pragma once

#include <vector>
#include "Types.h"

const int MIN_MAZE_SIZE = 10;
const int MAX_MAZE_SIZE = 50;
const int MAX_HINTS = 3;
const int HINT_PENALTY_SECONDS = 5;

bool isValidMazeSize(int size);

// Which challenges a mode has. Chaos has all of them.
bool modeHasFog(GameMode mode);
bool modeHasMoveLimit(GameMode mode);
bool modeHasKeys(GameMode mode);

// Builds a new random maze for the mode and level and starts the timer.
void startNewGame(GameState& state, int size, GameMode mode, Difficulty difficulty);
void endGame(GameState& state);

void restartTimer(GameState& state);                          // after the 3-2-1 countdown
void continueAfterPause(GameState& state, int pausedSeconds); // the pause does not count
int getElapsedSeconds(const GameState& state);                // hint penalties included

bool hasPlayerWon(const GameState& state);
bool hasPlayerLost(const GameState& state);   // no moves left
int getMovesLeft(const GameState& state);

// In fog modes only nearby tiles are visible. Keys, the exit and the snake's own trail always are.
bool isTileVisible(const GameState& state, int row, int col);

int getHintsLeft(const GameState& state);
vector<Position> useHint(GameState& state);   // the way to the next key or the exit; empty if no hints left

int getProgressPercent(const GameState& state);   // 0 at the start, 100 on the exit
int getStarRating(const GameState& state);        // 1-3 stars: fewer moves = more stars
