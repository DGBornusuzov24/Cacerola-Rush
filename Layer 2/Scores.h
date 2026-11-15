#pragma once

#include <vector>
#include "data/Types.h"

// Bigger mazes and harder levels give more points, a faster time gives more points.
int calculateScore(int size, Difficulty difficulty, int seconds);

ScoreEntry makeScoreEntry(const GameState& state, const string& name, int seconds);

// Adds the entry to the list and returns its rank (1 = best) among the games of the same mode.
int addScore(vector<ScoreEntry>& scores, const ScoreEntry& entry);

// The best games of one mode, best score first.
vector<ScoreEntry> getTopScores(const vector<ScoreEntry>& scores, GameMode mode, int count);

// The fastest time for the same mode + difficulty + size (the time to beat), or -1 if nobody played it yet.
int findBestTime(const vector<ScoreEntry>& scores, GameMode mode, Difficulty difficulty, int size);

bool isNewRecord(int bestTime, int seconds);
