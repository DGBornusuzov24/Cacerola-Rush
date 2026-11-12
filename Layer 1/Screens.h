#pragma once

#include <vector>
#include "data/Types.h"
using namespace std;

// 'bestBefore' is the old time to beat (-1 = nobody played this setup yet).
void showWinScreen(const ScoreEntry& entry, int rank, int bestBefore);
void showLoseScreen(const GameState& state);

// The top 10 of every mode. Left / Right switches the mode.
void showLeaderboard(const vector<ScoreEntry>& scores);
