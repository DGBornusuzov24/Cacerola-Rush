#pragma once

#include <string>
#include <vector>
#include "Types.h"
using namespace std;

const int HINT_LENGTH = 15;   // how many tiles of a hint are shown

// The pictures of the game (the help screen uses them too).
string getSnakeHeadGlyph();
string getWallGlyph(GameMode mode, int row, int mazeSize);
string getDoorGlyph();
string getKeyGlyph();
string getGateGlyph();
string getExitGlyph(bool bright);
string getTrailGlyph();
string getHintGlyph();

// Draws the maze and the info panel next to it in the middle of the window.
// Mazes that do not fit in the window scroll together with the snake.
void drawGame(const GameState& state, const string& message, int bestTime, const vector<Position>& hint);
