#pragma once

#include <string>
#include "data/Types.h"
using namespace std;

// The pictures of the game (the help screen uses them too).
string getSnakeHeadGlyph();
string getWallGlyph(GameMode mode, int row, int mazeSize);
string getDoorGlyph();
string getExitGlyph(bool bright);

// Draws the game screen in the middle of the terminal. 'bestTime' is the time to beat (-1 = none yet).
// Mazes that do not fit in the window scroll together with the snake.
void drawGame(const GameState& state, const string& message, int bestTime);